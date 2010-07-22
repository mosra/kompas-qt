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
 * @brief Class Map2X::QtGui::GraphicsMapView
 */

#include <QtGui/QGraphicsScene>

#include "../AbstractMapView.h"

namespace Map2X { namespace QtGui { namespace Plugins {

class MapView;
class Tile;

/**
 * @brief Map viewer using QGraphicsView
 *
 * @todo Tile loading and tile not found images for all tile sizes
 * @todo Better handling of resize event - map is loaded twice at startup!
 * @todo Overlay support (Tile as QGraphicsItemGroup, not QGraphicsPixmapItem)
 * @todo Remove reload() and replace with yet existing functions
 * @todo Sometimes laaaarge pause before map tile is loaded (why?)
 * @todo Do not delete tiles, just reposition and reload pixmaps
 * @todo Separation of every plugin into another dir? with NS?
 * @todo Non-crashing state when no model, no zoom or no layer is set
 */
class GraphicsMapView: public AbstractMapView {
    Q_OBJECT

    private:
        Core::Zoom _zoom;
        QString _layer;
        QStringList _overlays;

    public:
        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param f         Window flags
         */
        GraphicsMapView(QWidget* parent = 0, Qt::WindowFlags f = 0);

        virtual inline unsigned int zoom() const { return _zoom; }
        virtual Core::Wgs84Coords coords();
        virtual QString layer() const { return _layer; }
        virtual QStringList overlays() const { return _overlays; }

    public slots:
        virtual bool zoomIn();
        virtual bool zoomOut();
        virtual bool zoomTo(Core::Zoom zoom);
        virtual bool setCoords(const Core::Wgs84Coords& coords);
        virtual bool move(Direction direction, unsigned int speed) { return false; }
        virtual bool setLayer(const QString& layer);
        virtual bool addOverlay(const QString& overlay);
        virtual bool removeOverlay(const QString& overlay);
        virtual void reload();

    private:
        MapView* view;                          /**< @brief Map view */
        QGraphicsScene map;                     /**< @brief Map scene */
        Core::Coords<unsigned int> tileCount;   /**< @brief Tile count for current view */
        QList<Tile*> tiles;                     /**< @brief All tiles */

        QPixmap tileNotFoundImage,              /**< @brief "Tile not found" image */
            tileLoadingImage;                   /**< @brief "Tile loading" image */

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
         * @brief Update tile data
         *
         * Updates data of tiles. Called after changing layers/overlays.
         */
        void updateTileData();

        /**
         * @brief Update tile positions
         *
         * Updates tile positions, removes invisible and adds new to blank
         * parts of view. Called after movement or zooming.
         * @todo More precisely calculate positions of tiles (works well when
         *      moving to bottom right, but sucks when moving to top left)
         */
        void updateTilePositions();

        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QPixmap& data);
        inline virtual void tileLoading(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
            tileData(layer, z, coords, tileLoadingImage);
        }
        virtual void tileNotFound(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
            tileData(layer, z, coords, tileNotFoundImage);
        }
};

}}}

PLUGIN_REGISTER_STATIC(GraphicsMapView, Map2X::QtGui::Plugins::GraphicsMapView, "cz.mosra.Map2X.QtGui.AbstractMapView/0.1")
PLUGIN_SET_NAME("Map viewer using QGraphicsView")
PLUGIN_FINISH

#endif
