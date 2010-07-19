#ifndef Map2X_QtGui_AbstractMapView_h
#define Map2X_QtGui_AbstractMapView_h
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
 * @brief Class Map2X::QtGui::AbstractMapView
 */

#include <QtCore/QMutex>
#include <QtGui/QWidget>

#include "PluginManager/definitions.h"
#include "AbstractTileModel.h"

namespace Map2X { namespace QtGui {

class TileDataThread;

/** @brief Abstract class for map viewer widget plugins */
class AbstractMapView: public QWidget {
    Q_OBJECT

    #ifndef QT_MOC_RUN
    PLUGIN_INTERFACE("cz.mosra.Map2X.QtGui.AbstractMapView/0.1")
    #endif

    public:
        /** @brief Direction for moving map */
        enum Direction {
            Top, TopLeft, Left, BottomLeft, Bottom, BottimRight, Right, TopRight
        };

        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param f         Window flags
         */
        AbstractMapView(QWidget* parent = 0, Qt::WindowFlags f = 0);

        /**
         * @brief Reload map view
         *
         * Reloads all map data and redraws whole map view from scratch.
         */
        virtual void reload() = 0;

        /** @{ @name Map data functions */

        /**
         * @brief Set tile model to the view
         * @param model     Tile model
         *
         * Calls refresh().
         */
        void setTileModel(Core::AbstractTileModel* model);

        /**
         * @brief Set map layer
         * @param layer     Layer
         * @return Whether the layer exists and can be set.
         *
         * Sets map layer from list provided by tile model.
         */
        virtual bool setLayer(const QString& layer) = 0;

        /**
         * @brief Current map layer
         * @return Currently set map layer
         */
        virtual QString layer() const = 0;

        /**
         * @brief Add map overlay
         * @param layer     Overlay
         * @return Whether the overlay exists and can be added.
         *
         * Adds overlay to map from list provided by tile model.
         */
        virtual bool addOverlay(const QString& layer) = 0;

        /**
         * @brief Remove map overlay
         * @param overlay   Overlay
         * @return If the overlay was added and can be removed, returns true.
         *      Otherwise returns false.
         */
        virtual bool removeOverlay(const QString& overlay) = 0;

        /**
         * @brief List of added overlays
         * @return All currently added overlays
         */
        virtual QStringList overlays() const = 0;

        /*@}*/

        /** @{ @name Map movement functions */

    public:
        /**
         * @brief Current zoom
         */
        virtual unsigned int zoom() const = 0;

        /** @brief Get current map coordinates */
        virtual Core::Wgs84Coords coords() = 0;

    public slots:
        /**
         * @brief Zoom map in
         * @return Whether the map can be zoomed in
         */
        virtual bool zoomIn() = 0;

        /**
         * @brief Zoom map out
         * @return Whether the map can be zoomed out
         */
        virtual bool zoomOut() = 0;

        /**
         * @brief Zoom map to given zoom
         * @param zoom      Zoom level
         * @return Whether the zoom level is available
         */
        virtual bool zoomTo(Core::Zoom zoom) = 0;

        /**
         * @brief Move map to given coordinates
         * @param coords Coordinates
         * @return Whether the map can be moved to given coordinates (they
         *      are valid and they aren't out of available map area).
         *
         * Moves the map so given coordinates are centered in view area.
         */
        virtual bool setCoords(const Core::Wgs84Coords& coords) = 0;

        /**
         * @brief Move map in given direction
         * @param direction Direction
         * @param speed     Speed in pps (pixels per second)
         * @return Whether the map can be moved (whether map area is available)
         */
        virtual bool move(Direction direction, unsigned int speed) = 0;

        /*@}*/

    protected:
        Core::AbstractTileModel* tileModel;     /**< @brief Tile model */
        QMutex tileModelMutex;                  /**< @brief Mutex for tile model */
        TileDataThread* tileDataThread;         /**< @brief Thread for downloading tile data */

    signals:
        /**
         * @brief Request for tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         *
         * Connected to TileDataThread::getTileData().
         */
        void getTileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords);

    protected slots:
        /**
         * @brief Tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         * @param data      Tile data
         */
        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QPixmap& data) = 0;

        /**
         * @brief Tile data downloading
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         */
        virtual void tileLoading(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) = 0;

        /**
         * @brief Tile data not found
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         */
        virtual void tileNotFound(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) = 0;
};

}}

#endif
