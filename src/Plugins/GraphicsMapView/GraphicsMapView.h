#ifndef Map2X_QtGui_GraphicsMapView_h
#define Map2X_QtGui_GraphicsMapView_h
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

/** @file
 * @brief Class Map2X::Plugins::GraphicsMapView
 */

#include <QtGui/QGraphicsScene>

#include "AbstractMapView.h"

namespace Map2X { namespace Plugins {

class MapView;
class Tile;

/**
 * @brief Map viewer using QGraphicsView
 *
 * @todo Tile loading and tile not found images for all tile sizes
 * @todo Better handling of resize event - map is loaded twice at startup!
 * @todo Remove reload() and replace with yet existing functions
 * @todo Sometimes laaaarge pause before map tile is loaded (why?)
 * @todo Non-crashing state when no model, no zoom or no layer is set
 * @todo Discard all downloads when changing whole tile data
 */
class GraphicsMapView: public QtGui::AbstractMapView {
    Q_OBJECT

    private:
        Core::Zoom _zoom;
        QString _layer;
        QStringList _overlays;

    public:
        /** @copydoc QtGui::AbstractMapView::AbstractMapView */
        GraphicsMapView(Map2X::PluginManager::AbstractPluginManager* manager, const std::string& plugin);

        virtual inline unsigned int zoom() const { return _zoom; }
        virtual Core::Wgs84Coords coords(const QPoint& pos = QPoint());
        virtual Core::AbsoluteArea<unsigned int> tilesInArea(const QRect& area = QRect());
        virtual QString layer() const { return _layer; }
        virtual QStringList overlays() const { return _overlays; }

    public slots:
        virtual void updateRasterModel();
        virtual bool zoomIn(const QPoint& pos = QPoint());
        virtual bool zoomOut(const QPoint& pos = QPoint());
        virtual bool zoomTo(Core::Zoom zoom, const QPoint& pos = QPoint());
        virtual bool setCoords(const Map2X::Core::Wgs84Coords& coords, const QPoint& pos = QPoint());
        virtual bool move(Direction direction, unsigned int speed) { return false; }
        virtual bool setLayer(const QString& layer);
        virtual bool addOverlay(const QString& overlay);
        virtual bool removeOverlay(const QString& overlay);

    protected:
        /**
         * @brief Mouse move event
         *
         * If mouse is over the map and no button is pressed, emits
         * currentCoordinates().
         */
        virtual void mouseMoveEvent(QMouseEvent* event);

    private:
        MapView* view;                          /**< @brief Map view */
        QGraphicsScene map;                     /**< @brief Map scene */
        Core::Coords<unsigned int> tileCount;   /**< @brief Tile count for current view */
        QList<Tile*> tiles;                     /**< @brief All tiles */

        QGraphicsTextItem* informativeText;     /**< @brief Informative text when view is not ready */

        QPixmap tileNotFoundImage,              /**< @brief "Tile not found" image */
            tileLoadingImage;                   /**< @brief "Tile loading" image */

        /**
         * @brief Whether the view is ready for displaying
         *
         * View is ready when is visible, has an tile model assigned and the
         * tile model has at least one zoom level and one map layer.
         */
        bool isReady();

    private slots:

        /**
         * @brief Update map area
         *
         * Updates available map area. Called after zooming, adding/removing
         * map packages or enabling/disabling online maps.
         */
        void updateMapArea();

        /**
         * @brief Update displayed tile count
         *
         * Updates count of tiles for current view. Called after resizing view.
         */
        void updateTileCount();

        /**
         * @brief Update tile positions
         *
         * Updates tile positions, removes invisible and adds new to blank
         * parts of view. Called after movement or zooming.
         * @todo More precisely calculate positions of tiles (works well when
         *      moving to bottom right, but sucks when moving to top left)
         */
        void updateTilePositions();

        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QByteArray& data) {
            QPixmap pixmap;
            pixmap.loadFromData(data);
            tileData(layer, z, coords, pixmap);
        }
        inline virtual void tileLoading(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
            tileData(layer, z, coords, tileLoadingImage);
        }
        virtual void tileNotFound(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
            tileData(layer, z, coords, tileNotFoundImage);
        }

    private:
        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QPixmap& data);
};

}}

#endif
