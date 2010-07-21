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

GraphicsMapView::GraphicsMapView(QWidget* parent, Qt::WindowFlags f): AbstractMapView(parent, f), _zoom(0), tileNotFoundImage(":/tileNotFound-256.png"), tileLoadingImage(":/tileLoading-256.png") {
    /* Graphics view */
    view = new MapView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(&map);

    /* Update tile positions on map move */
    connect(view, SIGNAL(mapMoved()), SLOT(updateTilePositions()));

    /* Single-widget layout */
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(view);
    setLayout(layout);

    /* Initalize after a while (causes align problems) */
    QTimer::singleShot(500, this, SLOT(reload()));
}

bool GraphicsMapView::zoomIn() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Check whether we can zoom in */
    vector<Zoom> z = tileModel->zoomLevels();
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(++it == z.end()) return false;

    locker.unlock();

    /* Get coordinates, zoom in, update map area */
    Wgs84Coords c = coords();
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c);

    return true;
}

bool GraphicsMapView::zoomOut() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Check whether we can zoom out */
    vector<Zoom> z = tileModel->zoomLevels();
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(it-- == z.begin()) return false;

    locker.unlock();

    /* Get coordinates, zoom out, update map area */
    Wgs84Coords c = coords();
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c);

    return true;
}

bool GraphicsMapView::zoomTo(Zoom zoom) {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Check whether given zoom exists */
    vector<Zoom> z = tileModel->zoomLevels();
    if(::find(z.begin(), z.end(), zoom) == z.end()) return false;

    locker.unlock();

    /* Get coordinates, zoom out, update map area */
    Wgs84Coords c = coords();
    _zoom = zoom;
    updateMapArea();

    /* Remove old tiles and set coordinates back */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();
    setCoords(c);

    return true;
}

Wgs84Coords GraphicsMapView::coords() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return Wgs84Coords();

    QPointF center = view->mapToScene(view->size().width()/2, view->size().height()/2);
    return tileModel->toWgs84(_zoom, RasterCoords(
        static_cast<unsigned int>(center.x())/tileModel->tileSize().x,
        static_cast<unsigned int>(center.y())/tileModel->tileSize().y,
        static_cast<unsigned int>(center.x())%tileModel->tileSize().x,
        static_cast<unsigned int>(center.y())%tileModel->tileSize().y
    ));
}

bool GraphicsMapView::setCoords(const Wgs84Coords& coords) {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Convert coordinates to raster */
    RasterCoords rc = tileModel->fromWgs84(_zoom, coords);

    /* Center map to that coordinates */
    view->centerOn(rc.x()*tileModel->tileSize().x+rc.moveX(),
                   rc.y()*tileModel->tileSize().y+rc.moveY());

    locker.unlock();

    /* Update tile positions */
    updateTilePositions();

    return true;
}

bool GraphicsMapView::setLayer(const QString& layer) {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Check whether given layer exists */
    vector<string> layers = tileModel->layers();
    if(::find(layers.begin(), layers.end(), layer.toStdString()) == layers.end())
        return false;

    locker.unlock();

    /* Update tile data */
    _layer = layer;
    updateTileData();

    return true;
}

bool GraphicsMapView::addOverlay(const QString& overlay) {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return false;

    /* Check whether given overlay exists */
    vector<string> layers = tileModel->overlays();
    if(::find(layers.begin(), layers.end(), overlay.toStdString()) == layers.end())
        return false;

    locker.unlock();

    /* Update tile data (if the overlay is not already set) */
    if(!_overlays.contains(overlay)) {
        _overlays.append(overlay);
        updateTileData();
    }

    return true;
}

bool GraphicsMapView::removeOverlay(const QString& overlay) {
    if(!tileModel ||!_overlays.contains(overlay)) return false;

    _overlays.removeAll(overlay);
    return true;
}

void GraphicsMapView::updateMapArea() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return;

    /* Compute tile area */
    unsigned int multiplier = pow(tileModel->zoomStep(), _zoom-tileModel->zoomLevels()[0]);

    /* Resize map to area */
    map.setSceneRect(tileModel->area().x*tileModel->tileSize().x*multiplier,
                     tileModel->area().y*tileModel->tileSize().x*multiplier,
                     tileModel->area().w*tileModel->tileSize().x*multiplier,
                     tileModel->area().h*tileModel->tileSize().x*multiplier);
}

void GraphicsMapView::updateTileCount() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return;

    /* Tile count for actual viewed area */
    tileCount = tileModel->tilesInArea(Coords<unsigned int>(
        view->visibleRegion().boundingRect().width(),
        view->visibleRegion().boundingRect().height()));

    /* If map area is smaller than view area, smaller tile count */
    if(tileCount.x < tileModel->area().w) tileCount.x = tileModel->area().w;
    if(tileCount.y < tileModel->area().h) tileCount.y = tileModel->area().h;

    locker.unlock();

    updateTilePositions();
}

void GraphicsMapView::updateTilePositions() {
    QMutexLocker locker(&tileModelMutex);

    if(!tileModel) return;

    QPointF viewed = view->mapToScene(0, 0);

    /* Origin of viewed tiles */
    Coords<unsigned int> tilesOrigin(
        static_cast<unsigned int>(viewed.x()/tileModel->tileSize().x),
        static_cast<unsigned int>(viewed.y()/tileModel->tileSize().y));

    QBitArray loadedItems(tileCount.x*tileCount.y, false);

    /* Foreach tiles and remove these which are not in area */
    for(int i = tiles.size()-1; i >= 0; --i) {
        if((tiles[i]->x() < tilesOrigin.x || tiles[i]->x() >= tilesOrigin.x+tileCount.x) ||
           (tiles[i]->y() < tilesOrigin.y || tiles[i]->y() >= tilesOrigin.y+tileCount.y)) {
            delete tiles[i];
            tiles.removeAt(i);
        } else {
            loadedItems.setBit((tiles[i]->y()-tilesOrigin.y)*tileCount.x+tiles[i]->x()-tilesOrigin.x, true);
        }
    }

    locker.unlock();

    /* Load unloaded tiles */
    for(int i = 0; i != loadedItems.size(); ++i) {
        if(loadedItems.at(i)) continue;
        emit getTileData(_layer, _zoom, TileCoords(tilesOrigin.x+i%tileCount.x, tilesOrigin.y+i/tileCount.x));
    }
}

void GraphicsMapView::updateTileData() {
    /* Delete old tiles and request new data */
    for(int i = tiles.size()-1; i >= 0; --i) {
        emit getTileData(_layer, _zoom, TileCoords(tiles[i]->x(), tiles[i]->y()));
        delete tiles[i];
        tiles.removeAt(i);
    }
}

void GraphicsMapView::tileData(const QString& layer, Zoom z, const TileCoords& coords, const QPixmap& data) {
    QMutexLocker locker(&tileModelMutex);

    /* Delete old "loading" tile */
    for(int i = tiles.size()-1; i >= 0; --i) {
        if(tiles[i]->coords() == coords) {
            delete tiles[i];
            tiles.removeAt(i);
        }
    }

    Tile* tile = new Tile(coords, data, 0, &map);
    tile->setOffset(coords.x*tileModel->tileSize().x, coords.y*tileModel->tileSize().y);
    tiles.append(tile);
}

void GraphicsMapView::reload() {
    QMutexLocker locker(&tileModelMutex);

    map.clear();

    /** @todo Weird cycles if not checking visibility */
    if(!tileModel || !isVisible() || tileModel->zoomLevels().empty()) return;

    /* Reset zoom, if the model doesn't have it */
    vector<Zoom> z = tileModel->zoomLevels();
    if(::find(z.begin(), z.end(), _zoom) == z.end()) _zoom = z[0];

    locker.unlock();

    updateMapArea();
    updateTileCount();
}

}}}
