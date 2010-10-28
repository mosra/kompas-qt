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

#include "MainWindow.h"
#include "AbstractProjection.h"
#include "MapView.h"
#include "Tile.h"

using namespace std;
using namespace Map2X::Core;
using namespace Map2X::QtGui;

PLUGIN_REGISTER_STATIC(GraphicsMapView, Map2X::Plugins::GraphicsMapView,
                       "cz.mosra.Map2X.QtGui.AbstractMapView/0.1")

namespace Map2X { namespace Plugins {

GraphicsMapView::GraphicsMapView(PluginManager::AbstractPluginManager* manager, const std::string& pluginName): AbstractMapView(manager, pluginName), _zoom(0), informativeText(0), tileNotFoundImage(":/tileNotFound-256.png"), tileLoadingImage(":/tileLoading-256.png") {
    /* Enable mouse tracking */
    setMouseTracking(true);

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
}

bool GraphicsMapView::zoomIn(const QPoint& pos) {
    if(!isReady()) return false;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    vector<Zoom> z = rasterModel->zoomLevels();
    MainWindow::instance()->unlockRasterModel();

    /* Check whether we can zoom in */
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(++it == z.end()) return false;

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
    if(!isReady()) return false;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    vector<Zoom> z = rasterModel->zoomLevels();
    MainWindow::instance()->unlockRasterModel();

    /* Check whether we can zoom out */
    vector<Zoom>::const_iterator it = ::find(z.begin(), z.end(), _zoom);
    if(it-- == z.begin()) return false;

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
    if(!isReady()) return false;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    vector<Zoom> z = rasterModel->zoomLevels();
    MainWindow::instance()->unlockRasterModel();

    /* Check whether given zoom exists */
    if(::find(z.begin(), z.end(), zoom) == z.end()) return false;

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
    if(!isReady()) return Wgs84Coords();

    /* Position where to get coordinates */
    QPointF center;
    if(pos.isNull())
        center = view->mapToScene(view->width()/2, view->height()/2);
    else
        center = view->mapToScene(pos);

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* The model doesn't have projection, return invalid coordinates */
    if(!rasterModel->projection()) {
        MainWindow::instance()->unlockRasterModel();
        return Wgs84Coords();
    }

    Wgs84Coords ret = rasterModel->projection()->toWgs84(Coords<double>(
        center.x()/(pow(rasterModel->zoomStep(), _zoom)*rasterModel->tileSize().x),
        center.y()/(pow(rasterModel->zoomStep(), _zoom)*rasterModel->tileSize().y)
    ));

    MainWindow::instance()->unlockRasterModel();

    return ret;
}

AbsoluteArea<unsigned int> GraphicsMapView::tilesInArea(const QRect& area) {
    QRect sceneArea;
    if(area.isNull()) {
        sceneArea.setTopLeft(view->mapToScene(visibleRegion().boundingRect().topLeft()).toPoint());
        sceneArea.setBottomRight(view->mapToScene(visibleRegion().boundingRect().bottomRight()).toPoint());
    } else {
        sceneArea.setTopLeft(view->mapToScene(area.topLeft()).toPoint());
        sceneArea.setBottomRight(view->mapToScene(area.bottomRight()).toPoint());
    }

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    TileSize tileSize = rasterModel->tileSize();
    MainWindow::instance()->unlockRasterModel();

    /* Fix cases where scene is smaller than viewed area */
    if(sceneArea.left() < 0) {
        sceneArea.setLeft(0);
        sceneArea.setRight(tileSize.x*tileCount.x-1);
    }
    if(sceneArea.top() < 0) {
        sceneArea.setTop(0);
        sceneArea.setBottom(tileSize.y*tileCount.y-1);
    }

    return AbsoluteArea<unsigned int>(
        sceneArea.left()/tileSize.x,
        sceneArea.top()/tileSize.y,
        sceneArea.right()/tileSize.x,
        sceneArea.bottom()/tileSize.y
    );
}

bool GraphicsMapView::setCoords(const Wgs84Coords& coords, const QPoint& pos) {
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

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* The model doesn't have projection, nothing to do */
    if(!rasterModel->projection()) {
        MainWindow::instance()->unlockRasterModel();
        return false;
    }

    /* Convert coordinates to raster */
    Coords<double> rc = rasterModel->projection()->fromWgs84(coords);

    /* Center map to that coordinates (moved by 'pos' distance from center) */
    view->centerOn(rc.x*pow(rasterModel->zoomStep(), _zoom)*rasterModel->tileSize().x-x,
                   rc.y*pow(rasterModel->zoomStep(), _zoom)*rasterModel->tileSize().y-y);

    MainWindow::instance()->unlockRasterModel();

    /* Update tile positions */
    updateTilePositions();

    return true;
}

bool GraphicsMapView::setLayer(const QString& layer) {
    if(layer == _layer) return true;
    if(!isReady()) return false;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    vector<string> layers = rasterModel->layers();
    MainWindow::instance()->unlockRasterModel();

    /* Check whether given layer exists */
    if(::find(layers.begin(), layers.end(), layer.toStdString()) == layers.end())
        return false;

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
    if(!isReady()) return false;

    /* Check whether given overlay exists */
    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    vector<string> layers = rasterModel->overlays();
    MainWindow::instance()->unlockRasterModel();

    if(::find(layers.begin(), layers.end(), overlay.toStdString()) == layers.end())
        return false;

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

void GraphicsMapView::mouseMoveEvent(QMouseEvent* event) {
    /* Capture event only when no button is pressed */
    if(event->buttons()) event->ignore();
    else {
        AbstractMapView::mouseMoveEvent(event);
        emit currentCoordinates(coords(event->pos()));
    }
}

bool GraphicsMapView::isReady() {
    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    bool is = rasterModel && !rasterModel->layers().empty() && !rasterModel->zoomLevels().empty();
    MainWindow::instance()->unlockRasterModel();

    return is;
}

void GraphicsMapView::updateMapArea() {
    if(!isReady()) return;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* Compute tile area */
    unsigned int multiplier = pow(rasterModel->zoomStep(), _zoom-rasterModel->zoomLevels()[0]);

    /* Resize map to area */
    map.setSceneRect(rasterModel->area().x*rasterModel->tileSize().x*multiplier,
                     rasterModel->area().y*rasterModel->tileSize().y*multiplier,
                     rasterModel->area().w*rasterModel->tileSize().x*multiplier,
                     rasterModel->area().h*rasterModel->tileSize().y*multiplier);

    MainWindow::instance()->unlockRasterModel();

    /* Update tile count to ensure map area fits in it */
    updateTileCount();
}

void GraphicsMapView::updateTileCount() {
    if(!isReady() || !isVisible()) return;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* Tile count for actual viewed area */
    tileCount = rasterModel->tilesInArea(Coords<unsigned int>(
        view->visibleRegion().boundingRect().width(),
        view->visibleRegion().boundingRect().height()));

    /* If map area is smaller than view area, set tile count to map area */
    unsigned int multiplier = pow(rasterModel->zoomStep(), _zoom-rasterModel->zoomLevels()[0]);
    if(tileCount.x > rasterModel->area().w*multiplier) tileCount.x = rasterModel->area().w*multiplier;
    if(tileCount.y > rasterModel->area().h*multiplier) tileCount.y = rasterModel->area().h*multiplier;

    MainWindow::instance()->unlockRasterModel();

    updateTilePositions();
}

void GraphicsMapView::updateTilePositions() {
    if(!isReady() || !isVisible()) return;

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    QPointF viewed = view->mapToScene(0, 0);

    /* Ensure positive coordinates */
    if(viewed.x() < 0) viewed.setX(0);
    if(viewed.y() < 0) viewed.setY(0);

    /* Origin of viewed tiles */
    Coords<unsigned int> tilesOrigin(
        static_cast<unsigned int>(viewed.x()/rasterModel->tileSize().x),
        static_cast<unsigned int>(viewed.y()/rasterModel->tileSize().y));

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
        tile->setPos(coords.x*rasterModel->tileSize().x, coords.y*rasterModel->tileSize().y);
        tiles.append(tile);

        /* Foreach all layers and overlays and request data for them */
        emit getTileData(_layer, _zoom, coords);
        foreach(const QString& overlay, _overlays)
            emit getTileData(overlay, _zoom, coords);
    }

    MainWindow::instance()->unlockRasterModel();
}

void GraphicsMapView::tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QPixmap& data) {
    /* Compute layer/overlay number */
    int layerNumber;
    if(layer == _layer) layerNumber = 0;
    else layerNumber = _overlays.indexOf(layer)+1;

    for(int i = tiles.size()-1; i >= 0; --i) if(tiles[i]->coords() == coords) {
        tiles[i]->setLayer(layerNumber, data);
        break;
    }
}

void GraphicsMapView::setRasterModel() {
    qDeleteAll(tiles);
    tiles.clear();

    /* If the view is not ready, display informative text  */
    if(!isReady()) {
        if(!informativeText) {
            informativeText = new QGraphicsTextItem(0, &map);
            informativeText->setHtml("<center>" + tr(
                "<strong>No map to display.</strong><br /><br />"
                "Please select a map and ensure the map has at least one "
                "zoom level and layer available."
            ) + "</center>");
            informativeText->setTextWidth(256);
        }

        return;
    }

    if(informativeText) {
        delete informativeText;
        informativeText = 0;
    }

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    vector<Zoom> z = rasterModel->zoomLevels();
    vector<string> l = rasterModel->layers();
    vector<string> o = rasterModel->overlays();

    MainWindow::instance()->unlockRasterModel();

    /* Reset zoom, if the model doesn't have current */
    /** @todo Reset to closest available zoom */
    if(::find(z.begin(), z.end(), _zoom) == z.end()) _zoom = z[0];

    /* Reset map layer, if the model doesn't have current */
    if(::find(l.begin(), l.end(), _layer.toStdString()) == l.end()) _layer = QString::fromStdString(l[0]);

    /* Reset map overlays, if the model doesn't have current */
    for(int i = _overlays.size()-1; i >= 0; --i) {
        if(::find(o.begin(), o.end(), _overlays[i].toStdString()) == o.end())
            _overlays.removeAt(i);
    }

    /** @todo Is this check really needed? Why don't just reset everything to defaults? */

    updateMapArea();
    updateTileCount();
}

}}
