/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "GraphicsMapView.h"

#include <vector>
#include <algorithm>
#include <QtCore/QTimer>
#include <QtCore/QBitArray>
#include <QtCore/QDebug>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGraphicsItem>
#include <QtGui/QMouseEvent>
#include <QtGui/QScrollBar>

#include "MainWindow.h"
#include "AbstractProjection.h"
#include "MapView.h"
#include "Tile.h"
#include "TileDataThread.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

PLUGIN_REGISTER_STATIC(GraphicsMapView, Kompas::Plugins::GraphicsMapView,
                       "cz.mosra.Kompas.QtGui.AbstractMapView/0.2")

namespace Kompas { namespace Plugins {

GraphicsMapView::GraphicsMapView(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractMapView(manager, plugin), _zoom(0), tileNotFoundImage(":/notfound-256.png"), tileLoadingImage(":/loading-256.png") {
    /* Enable mouse tracking */
    setMouseTracking(true);

    /* Graphics view */
    view = new MapView(this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(&map);

    map.setBackgroundBrush(QColor("#e6e6e6"));

    /* Update tile positions on map move */
    connect(view, SIGNAL(mapMoved()), SLOT(updateTilePositions()));
    connect(view, SIGNAL(mapResized()), SLOT(updateTileCount()));

    /* Zoom in/out on wheel event */
    connect(view, SIGNAL(zoomIn(QPoint)), SLOT(zoomIn(QPoint)));
    connect(view, SIGNAL(zoomOut(QPoint)), SLOT(zoomOut(QPoint)));

    /* Single-widget layout */
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    updateRasterModel();
}

bool GraphicsMapView::zoomIn(const QPoint& pos) {
    if(!isReady()) return false;

    set<Zoom> z = MainWindow::instance()->rasterModelForRead()()->zoomLevels();

    /* Check whether we can zoom in */
    set<Zoom>::const_iterator it = z.find(_zoom);
    if(++it == z.end()) return false;

    /* Abort all jobs from previous zoom */
    tileDataThread->abort();

    /* Get the coordinates before zooming */
    QPointF move;
    if(!pos.isNull())  {
        move.setX(pos.x()-view->width()/2);
        move.setY(pos.y()-view->height()/2);
    }
    QPointF coords = view->mapToScene(view->width()/2, view->height()/2)+move;

    /* Zoom in, update map area */
    unsigned int multiplier = pow2(*it-_zoom);
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();

    /* Center on multiplied position, count with 'pos' distance from center */
    view->centerOn(coords*multiplier-move);

    /* Load new tiles */
    updateTileCount();

    emit zoomChanged(_zoom);

    return true;
}

bool GraphicsMapView::zoomOut(const QPoint& pos) {
    if(!isReady()) return false;

    set<Zoom> z = MainWindow::instance()->rasterModelForRead()()->zoomLevels();

    /* Check whether we can zoom out */
    set<Zoom>::const_iterator it = z.find(_zoom);
    if(it-- == z.begin()) return false;

    /* Abort all jobs from previous zoom */
    tileDataThread->abort();

    /* Get coordinates before we zoom out (so they don't get cropped) */
    QPointF move;
    if(!pos.isNull())  {
        move.setX(pos.x()-view->width()/2);
        move.setY(pos.y()-view->height()/2);
    }
    QPointF coords = view->mapToScene(view->width()/2, view->height()/2)+move;

    /* Zoom out, update map area */
    unsigned int divisor = pow2(_zoom-*it);
    _zoom = *it;
    updateMapArea();

    /* Remove old tiles */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();

    /* Center on divided position, count with 'pos' distance from center */
    view->centerOn(coords/divisor-move);

    /* Load new tiles */
    updateTileCount();

    emit zoomChanged(_zoom);

    return true;
}

bool GraphicsMapView::zoomTo(Core::Zoom zoom, const QPoint& pos) {
    if(!isReady()) return false;

    /* If we are at the zoom already, nothing to do */
    if(zoom == _zoom) return true;

    set<Zoom> z = MainWindow::instance()->rasterModelForRead()()->zoomLevels();

    /* Check whether given zoom exists */
    if(z.find(zoom) == z.end()) return false;

    /* Abort all jobs from previous zoom */
    tileDataThread->abort();

    /* Get coordinates before we zoom (so they don't get cropped) */
    QPointF move;
    if(!pos.isNull())  {
        move.setX(pos.x()-view->width()/2);
        move.setY(pos.y()-view->height()/2);
    }
    QPointF coords = view->mapToScene(view->width()/2, view->height()/2)+move;

    /* Zoom, update map area */
    if(_zoom > zoom) coords /= pow2(_zoom-zoom);
    else coords *= pow2(zoom-_zoom);
    _zoom = zoom;
    updateMapArea();

    /* Remove old tiles */
    qDeleteAll<QList<Tile*> >(tiles);
    tiles.clear();

    /* Center on divided position, count with 'pos' distance from center */
    view->centerOn(coords-move);

    /* Load new tiles */
    updateTileCount();

    emit zoomChanged(_zoom);

    return true;
}

LatLonCoords GraphicsMapView::coords(const QPoint& pos) {
    if(!isReady()) return LatLonCoords();

    /* Position where to get coordinates */
    QPointF center;
    if(pos.isNull())
        center = view->mapToScene(view->width()/2, view->height()/2);
    else
        center = view->mapToScene(pos);

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* The model doesn't have projection, return invalid coordinates */
    if(!rasterModel()->projection()) return LatLonCoords();

    LatLonCoords ret = rasterModel()->projection()->toLatLon(Coords<double>(
        center.x()/(pow2(_zoom)*rasterModel()->tileSize().x),
        center.y()/(pow2(_zoom)*rasterModel()->tileSize().y)
    ));

    return ret;
}

AbsoluteArea<double> GraphicsMapView::viewedArea(const QRect& area) {
    QRect sceneArea;
    if(area.isNull()) {
        sceneArea.setTopLeft(view->mapToScene(0, 0).toPoint());
        sceneArea.setBottomRight(view->mapToScene(width(), height()).toPoint());
    } else {
        sceneArea.setTopLeft(view->mapToScene(area.topLeft()).toPoint());
        sceneArea.setBottomRight(view->mapToScene(area.bottomRight()).toPoint());
    }

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    TileSize tileSize = rasterModel()->tileSize();
    TileArea a = rasterModel()->area()*tileSize*pow2(_zoom-*rasterModel()->zoomLevels().begin());
    rasterModel.unlock();

    /* Fix cases where scene is smaller than viewed area */
    if(sceneArea.left() < 0) {
        sceneArea.setLeft(0);
        sceneArea.setRight(tileSize.x*tileCount.x);
    }
    if(sceneArea.top() < 0) {
        sceneArea.setTop(0);
        sceneArea.setBottom(tileSize.y*tileCount.y);
    }

    return AbsoluteArea<double>(
        static_cast<double>(sceneArea.left()-a.x)/a.w,
        static_cast<double>(sceneArea.top()-a.y)/a.h,
        static_cast<double>(sceneArea.right()-a.x)/a.w,
        static_cast<double>(sceneArea.bottom()-a.y)/a.h
    );
}

bool GraphicsMapView::setCoords(const LatLonCoords& coords, const QPoint& pos) {
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

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* The model doesn't have projection, nothing to do */
    if(!rasterModel()->projection()) return false;

    /* Convert coordinates to raster */
    Coords<double> rc = rasterModel()->projection()->fromLatLon(coords);

    /* Center map to that coordinates (moved by 'pos' distance from center) */
    view->centerOn(rc.x*pow2(_zoom)*rasterModel()->tileSize().x-x,
                   rc.y*pow2(_zoom)*rasterModel()->tileSize().y-y);
    rasterModel.unlock();

    /* Update tile positions */
    updateTileCount();

    return true;
}

bool GraphicsMapView::move(int x, int y) {
    view->horizontalScrollBar()->setValue(view->horizontalScrollBar()->value()+x);
    view->verticalScrollBar()->setValue(view->verticalScrollBar()->value()+y);
    updateTilePositions();

    return true;
}

bool GraphicsMapView::setLayer(const QString& layer) {
    if(layer == _layer) return true;
    if(!isReady()) return false;

    /* Abort all jobs with current layer */
    tileDataThread->abort(_layer);

    vector<string> layers = MainWindow::instance()->rasterModelForRead()()->layers();

    /* Check whether given layer exists */
    if(::find(layers.begin(), layers.end(), layer.toStdString()) == layers.end())
        return false;

    /* Update tile data */
    _layer = layer;
    foreach(Tile* tile, tiles) {
        /* Placeholder for new data */
        tile->setLayer(0);

        tileDataThread->getTileData(_layer, _zoom, tile->coords());
    }

    emit layerChanged(_layer);
    return true;
}

bool GraphicsMapView::addOverlay(const QString& overlay) {
    if(_overlays.contains(overlay)) return true;
    if(!isReady()) return false;

    /* Check whether given overlay exists */
    vector<string> layers = MainWindow::instance()->rasterModelForRead()()->overlays();

    if(::find(layers.begin(), layers.end(), overlay.toStdString()) == layers.end())
        return false;

    _overlays.append(overlay);

    int layerNumber = _overlays.size();
    foreach(Tile* tile, tiles) {
        /* Add placeholder for new layer */
        tile->setLayer(layerNumber);

        /* Request new layer data */
        tileDataThread->getTileData(overlay, _zoom, tile->coords());
    }

    emit overlaysChanged(_overlays);
    return true;
}

bool GraphicsMapView::removeOverlay(const QString& overlay) {
    if(!isReady() ||!_overlays.contains(overlay)) return false;

    /* Abort all jobs with that overlay */
    tileDataThread->abort(overlay);

    int layerNumber = _overlays.indexOf(overlay);

    _overlays.removeAt(layerNumber);
    foreach(Tile* tile, tiles)
        tile->removeLayer(layerNumber+1);

    emit overlaysChanged(_overlays);
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

void GraphicsMapView::updateMapArea() {
    if(!isReady()) return;

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* Compute tile area */
    unsigned int multiplier = pow2(_zoom-*rasterModel()->zoomLevels().begin());

    /* Resize map to area */
    map.setSceneRect(rasterModel()->area().x*rasterModel()->tileSize().x*multiplier,
                     rasterModel()->area().y*rasterModel()->tileSize().y*multiplier,
                     rasterModel()->area().w*rasterModel()->tileSize().x*multiplier,
                     rasterModel()->area().h*rasterModel()->tileSize().y*multiplier);
}

void GraphicsMapView::updateTileCount() {
    if(!isReady() || !isVisible()) return;

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* Tile count for actual viewed area */
    tileCount = rasterModel()->tilesInArea(Coords<unsigned int>(
        view->visibleRegion().boundingRect().width(),
        view->visibleRegion().boundingRect().height()));

    /* If map area is smaller than view area, set tile count to map area */
    unsigned int multiplier = pow2(_zoom-*rasterModel()->zoomLevels().begin());
    if(tileCount.x > rasterModel()->area().w*multiplier) tileCount.x = rasterModel()->area().w*multiplier;
    if(tileCount.y > rasterModel()->area().h*multiplier) tileCount.y = rasterModel()->area().h*multiplier;

    rasterModel.unlock();

    updateTilePositions();
}

void GraphicsMapView::updateTilePositions() {
    if(!isReady() || !isVisible()) return;

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    TileArea area = rasterModel()->area()*pow2(_zoom-*rasterModel()->zoomLevels().begin());
    TileSize tileSize = rasterModel()->tileSize();
    rasterModel.unlock();

    QPointF viewed = view->mapToScene(0, 0);

    /* Ensure positive coordinates */
    if(viewed.x() < 0) viewed.setX(0);
    if(viewed.y() < 0) viewed.setY(0);

    /* Origin of viewed tiles */
    Coords<unsigned int> tilesOrigin(
        static_cast<unsigned int>(viewed.x()/tileSize.x),
        static_cast<unsigned int>(viewed.y()/tileSize.y));

    /* Ensure coordinates fit into map area */
    if(tilesOrigin.x < area.x)
        tilesOrigin.x = area.x;
    if(tilesOrigin.y < area.y)
        tilesOrigin.y = area.y;
    if(tilesOrigin.x + tileCount.x >= area.x+area.w)
        tilesOrigin.x = area.x+area.w-tileCount.x;
    if(tilesOrigin.y + tileCount.y >= area.y+area.h)
        tilesOrigin.y = area.y+area.h-tileCount.y;

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
        Tile* tile = new Tile(tileSize, coords, 0, &map);
        tile->setPos(coords.x*tileSize.x, coords.y*tileSize.y);
        tiles.append(tile);

        /* Foreach all layers and overlays and request data for them */
        tileDataThread->getTileData(_layer, _zoom, coords);
        foreach(const QString& overlay, _overlays)
            tileDataThread->getTileData(overlay, _zoom, coords);
    }
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

void GraphicsMapView::updateRasterModel() {
    if(!isReady()) return;

    /**
     * @bug Weird behaviour when enabling / disabling online maps,
     * unnoticed since commit ba9582a9d46a9534d3da9a265c145dab9a10d719 ?
     * Another (probably unrelated) problem is that when enabling online map
     * when there is an package with all the data available, the data are still
     * loaded!
     */

    qDeleteAll(tiles);
    tiles.clear();
    map.setSceneRect(0, 0, 0, 0);
    _zoom = pow2(31); /* << something impossible, so zoomTo won't skip on z == _zoom */
    _layer.clear();
    _overlays.clear();

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    Zoom z = *rasterModel()->zoomLevels().begin();
    QString layer = QString::fromStdString(rasterModel()->layers()[0]);
    QString _copyright = QString::fromStdString(rasterModel()->copyright());
    rasterModel.unlock();

    updateMapArea();

    /* Zoom and set layer (that emits signals), emit signal about overlays
       cleared too */
    zoomTo(z);
    setLayer(layer);
    emit overlaysChanged(_overlays);

    QRectF rect = map.sceneRect();
    view->centerOn(rect.left() + rect.width()/2, rect.top() + rect.height()/2);
    updateTilePositions();

    /** @todo @c VERSION-0.1.1 Stay on previous coordinates, nearest zoom level (previous model needed)  */
}

}}
