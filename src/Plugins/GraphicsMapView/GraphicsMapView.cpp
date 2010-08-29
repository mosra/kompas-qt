/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "GraphicsMapView.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <QtCore/QTimer>
#include <QtCore/QBitArray>
#include <QtCore/QDebug>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGraphicsItem>
#include <QtGui/QMouseEvent>

#include "MapView.h"
#include "Tile.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui { namespace Plugins {

GraphicsMapView::GraphicsMapView(PluginManager::AbstractPluginManager* manager, const std::string& pluginName): AbstractMapView(manager, pluginName), _zoom(0), informativeText(0), tileNotFoundImage(":/tileNotFound-256.png"), tileLoadingImage(":/tileLoading-256.png") {
    /* Graphics view */
    view = new MapView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(&map);

    /* Update tile positions on map move */
    connect(view, SIGNAL(mapMoved()), SLOT(updateTilePositions()));
    connect(view, SIGNAL(mapResized()), SLOT(updateTileCount()));

    /* Zoom in/out on wheel event */
    connect(view, SIGNAL(zoomIn(QPoint)), SLOT(zoomIn(QPoint)));
    connect(view, SIGNAL(zoomOut(QPoint)), SLOT(zoomOut(QPoint)));

    /* Single-widget layout */
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(view);
    setLayout(layout);

    /* Initalize after a while (causes align problems) */
    QTimer::singleShot(500, this, SLOT(reload()));
}

bool GraphicsMapView::zoomIn(const QPoint& pos) {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Check whether we can zoom in */
    vector<Zoom> z = tileModel->zoomLevels();
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(++it == z.end()) return false;

    locker.unlock();

    /* Get coordinates, zoom in, update map area */
    Wgs84Coords c = coords(pos);
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c, pos);

    return true;
}

bool GraphicsMapView::zoomOut(const QPoint& pos) {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Check whether we can zoom out */
    vector<Zoom> z = tileModel->zoomLevels();
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(it-- == z.begin()) return false;

    locker.unlock();

    /* Get coordinates, zoom out, update map area */
    Wgs84Coords c = coords(pos);
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c, pos);

    return true;
}

bool GraphicsMapView::zoomTo(Core::Zoom zoom, const QPoint& pos) {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Check whether given zoom exists */
    vector<Zoom> z = tileModel->zoomLevels();
    if(::find(z.begin(), z.end(), zoom) == z.end()) return false;

    locker.unlock();

    /* Get coordinates, zoom out, update map area */
    Wgs84Coords c = coords(pos);
    _zoom = zoom;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c, pos);

    return true;
}

Wgs84Coords GraphicsMapView::coords(const QPoint& pos) {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return Wgs84Coords();

    /* Position where to get coordinates */
    QPointF center;
    if(pos.isNull())
        center = view->mapToScene(view->width()/2, view->height()/2);
    else
        center = view->mapToScene(pos);

    return tileModel->toWgs84(_zoom, RasterCoords(
        static_cast<unsigned int>(center.x())/tileModel->tileSize().x,
        static_cast<unsigned int>(center.y())/tileModel->tileSize().y,
        static_cast<unsigned int>(center.x())%tileModel->tileSize().x,
        static_cast<unsigned int>(center.y())%tileModel->tileSize().y
    ));
}

bool GraphicsMapView::setCoords(const Wgs84Coords& coords, const QPoint& pos) {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Distance of 'pos' from map center */
    int x, y;
    if(pos.isNull()) {
        x = 0;
        y = 0;
    } else {
        x = pos.x()-view->width()/2;
        y = pos.y()-view->height()/2;
    }

    /* Convert coordinates to raster */
    RasterCoords rc = tileModel->fromWgs84(_zoom, coords);

    /* Center map to that coordinates (moved by 'pos' distance from center) */
    view->centerOn(rc.x()*tileModel->tileSize().x+rc.moveX()-x,
                   rc.y()*tileModel->tileSize().y+rc.moveY()-y);

    locker.unlock();

    /* Update tile positions */
    updateTilePositions();

    return true;
}

bool GraphicsMapView::setLayer(const QString& layer) {
    if(layer == _layer) return true;

    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Check whether given layer exists */
    vector<string> layers = tileModel->layers();
    if(::find(layers.begin(), layers.end(), layer.toStdString()) == layers.end())
        return false;

    locker.unlock();

    /* Update tile data */
    _layer = layer;
    foreach(Tile* tile, tiles) {
        /* Placeholder for new data */
        tile->setLayer(0);

        emit getTileData(_layer, _zoom, tile->coords());
    }

    return true;
}

bool GraphicsMapView::addOverlay(const QString& overlay) {
    if(_overlays.contains(overlay)) return true;

    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return false;

    /* Check whether given overlay exists */
    vector<string> layers = tileModel->overlays();
    if(::find(layers.begin(), layers.end(), overlay.toStdString()) == layers.end())
        return false;

    locker.unlock();

    _overlays.append(overlay);

    int layerNumber = _overlays.size();
    foreach(Tile* tile, tiles) {
        /* Add placeholder for new layer */
        tile->setLayer(layerNumber);

        /* Request new layer data */
        emit getTileData(overlay, _zoom, tile->coords());
    }

    return true;
}

bool GraphicsMapView::removeOverlay(const QString& overlay) {
    if(!isReady() ||!_overlays.contains(overlay)) return false;

    int layerNumber = _overlays.indexOf(overlay);

    _overlays.removeAt(layerNumber);
    foreach(Tile* tile, tiles)
        tile->removeLayer(layerNumber+1);

    return true;
}

bool GraphicsMapView::isReady() {
    return tileModel && !tileModel->layers().empty() && !tileModel->zoomLevels().empty();
}

void GraphicsMapView::updateMapArea() {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady()) return;

    /* Compute tile area */
    unsigned int multiplier = pow(tileModel->zoomStep(), _zoom-tileModel->zoomLevels()[0]);

    /* Resize map to area */
    map.setSceneRect(tileModel->area().x*tileModel->tileSize().x*multiplier,
                     tileModel->area().y*tileModel->tileSize().x*multiplier,
                     tileModel->area().w*tileModel->tileSize().x*multiplier,
                     tileModel->area().h*tileModel->tileSize().x*multiplier);

    locker.unlock();

    /* Update tile count to ensure map area fits in it */
    updateTileCount();
}

void GraphicsMapView::updateTileCount() {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady() || !isVisible()) return;

    /* Tile count for actual viewed area */
    tileCount = tileModel->tilesInArea(Coords<unsigned int>(
        view->visibleRegion().boundingRect().width(),
        view->visibleRegion().boundingRect().height()));

    /* If map area is smaller than view area, set tile count to map area */
    unsigned int multiplier = pow(tileModel->zoomStep(), _zoom-tileModel->zoomLevels()[0]);
    if(tileCount.x > tileModel->area().w*multiplier) tileCount.x = tileModel->area().w*multiplier;
    if(tileCount.y > tileModel->area().h*multiplier) tileCount.y = tileModel->area().h*multiplier;

    locker.unlock();

    updateTilePositions();
}

void GraphicsMapView::updateTilePositions() {
    QMutexLocker locker(&tileModelMutex);

    if(!isReady() || !isVisible()) return;

    QPointF viewed = view->mapToScene(0, 0);

    /* Ensure positive coordinates */
    if(viewed.x() < 0) viewed.setX(0);
    if(viewed.y() < 0) viewed.setY(0);

    /* Origin of viewed tiles */
    Coords<unsigned int> tilesOrigin(
        static_cast<unsigned int>(viewed.x()/tileModel->tileSize().x),
        static_cast<unsigned int>(viewed.y()/tileModel->tileSize().y));

    locker.unlock();

    QBitArray loadedItems(tileCount.x*tileCount.y, false);

    /* Foreach tiles and remove these which are not in area */
    for(int i = tiles.size()-1; i >= 0; --i) {
        if((tiles[i]->coords().x < tilesOrigin.x || tiles[i]->coords().x >= tilesOrigin.x+tileCount.x) ||
           (tiles[i]->coords().y < tilesOrigin.y || tiles[i]->coords().y >= tilesOrigin.y+tileCount.y)) {
            delete tiles[i];
            tiles.removeAt(i);
        } else {
            loadedItems.setBit((tiles[i]->coords().y-tilesOrigin.y)*tileCount.x+tiles[i]->coords().x-tilesOrigin.x, true);
        }
    }

    /* Create non-existent tiles */
    for(int i = 0; i != loadedItems.size(); ++i) {
        if(loadedItems.at(i)) continue;

        TileCoords coords(tilesOrigin.x+i%tileCount.x, tilesOrigin.y+i/tileCount.x);

        /* Create new tile at given position */
        Tile* tile = new Tile(coords, 0, &map);
        tile->setPos(coords.x*tileModel->tileSize().x, coords.y*tileModel->tileSize().y);
        tiles.append(tile);

        /* Foreach all layers and overlays and request data for them */
        emit getTileData(_layer, _zoom, coords);
        foreach(const QString& overlay, _overlays)
            emit getTileData(overlay, _zoom, coords);
    }
}

void GraphicsMapView::tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QPixmap& data) {
    QMutexLocker locker(&tileModelMutex);

    /* Compute layer/overlay number */
    int layerNumber;
    if(layer == _layer) layerNumber = 0;
    else layerNumber = _overlays.indexOf(layer)+1;

    for(int i = tiles.size()-1; i >= 0; --i) if(tiles[i]->coords() == coords) {
        tiles[i]->setLayer(layerNumber, data);
        break;
    }
}

void GraphicsMapView::reload() {
    QMutexLocker locker(&tileModelMutex);

    qDeleteAll(tiles);
    tiles.clear();

    /* If the view is not ready, display informative text  */
    if(!isReady()) {
        informativeText = new QGraphicsTextItem(0, &map);
        informativeText->setHtml("<center>" + tr(
            "<strong>No map to display.</strong><br /><br />"
            "Please select a map and ensure the map has at least one "
            "zoom level and layer available."
        ) + "</center>");
        informativeText->setTextWidth(256);

        return;
    }

    if(informativeText) {
        delete informativeText;
        informativeText = 0;
    }

    /* Reset zoom, if the model doesn't have current */
    vector<Zoom> z = tileModel->zoomLevels();
    if(::find(z.begin(), z.end(), _zoom) == z.end()) _zoom = z[0];

    /* Reset map layer, if the model doesn't have current */
    vector<string> l = tileModel->layers();
    if(::find(l.begin(), l.end(), _layer.toStdString()) == l.end()) _layer = QString::fromStdString(l[0]);

    /* Reset map overlays, if the model doesn't have current */
    vector<string> o = tileModel->overlays();
    for(int i = _overlays.size()-1; i >= 0; --i) {
        if(::find(o.begin(), o.end(), _overlays[i].toStdString()) == o.end())
            _overlays.removeAt(i);
    }

    /** @todo Is this check really needed? Why don't just reset everything to defaults? */

    locker.unlock();

    updateMapArea();
    updateTileCount();
}

}}}
