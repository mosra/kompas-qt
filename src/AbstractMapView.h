#ifndef Kompas_QtGui_AbstractMapView_h
#define Kompas_QtGui_AbstractMapView_h
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

/** @file
 * @brief Class Kompas::QtGui::AbstractMapView
 */

#include <QtGui/QWidget>

#include "AbsoluteArea.h"
#include "AbstractRasterModel.h"

class QAction;
class QMenu;

namespace Kompas { namespace QtGui {

class TileDataThread;

/** @brief Abstract class for map viewer widget plugins */
class AbstractMapView: public QWidget, public Corrade::PluginManager::Plugin {
    Q_OBJECT

    #ifndef QT_MOC_RUN
    PLUGIN_INTERFACE("cz.mosra.Kompas.QtGui.AbstractMapView/0.2")
    #endif

    public:
        /** @copydoc PluginManager::Plugin::Plugin */
        AbstractMapView(Corrade::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        /** @{ @name Map data functions */

    public slots:
        /**
         * @brief Update raster model
         * @param previous  If set, the model is changed to another. Otherwise
         *      just a new package was loaded or online maps enabled / disabled.
         *
         * Connected to MainWindow::rasterModelChanged(). Loads map data from
         * new model.
         */
        virtual void updateRasterModel(const Core::AbstractRasterModel* previous = 0) = 0;

    public:
        /**
         * @brief Set map layer
         * @param layer     Layer
         * @return Whether the layer exists and can be set.
         *
         * Sets map layer from list provided by tile model. Subclasses should
         * emit layerChanged() signal on success.
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
         * Adds overlay to map from list provided by tile model. Subclasses
         * should emit overlaysChanged() signal on success.
         */
        virtual bool addOverlay(const QString& layer) = 0;

        /**
         * @brief Remove map overlay
         * @param overlay   Overlay
         * @return If the overlay was added and can be removed, returns true.
         *      Otherwise returns false.
         *
         * Subclasses should emit overlaysChanged() signal on success.
         */
        virtual bool removeOverlay(const QString& overlay) = 0;

        /**
         * @brief List of added overlays
         * @return All currently added overlays
         */
        virtual QStringList overlays() const = 0;

        /*@}*/

        /** @{ @name Map movement functions */

        /**
         * @brief Current zoom
         */
        virtual unsigned int zoom() const = 0;

        /**
         * @brief Get current map coordinates
         * @param pos       Position for which compute the coordinates, relative
         *      to map view widget. If the position is null, the coordinates
         *      should be taken from center of view widget.
         */
        virtual Core::LatLonCoords coords(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Currently viewed area
         * @param area      Area relative to map view widget. If given area is
         *      null, the function returns coordinates for whole visible area
         *      in map view widget.
         * @return Coordinates of the area as portion of whole map area.
         *      If the map is smaller than view area, returns (0, 1) for that
         *      direction.
         */
        virtual Core::AbsoluteArea<double> viewedArea(const QRect& area = QRect()) = 0;

    public slots:
        /**
         * @brief Zoom map in
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the map can be zoomed in
         *
         * Subclasses should emit zoomChanged() signal on success.
         */
        virtual bool zoomIn(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Zoom map out
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the map can be zoomed out
         *
         * Subclasses should emit zoomChanged() signal on success.
         */
        virtual bool zoomOut(const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Zoom map to given zoom
         * @param zoom      Zoom level
         * @param pos       Position which should keep its coordinates after
         *      zooming, relative to map view widget. Useful for zooming to
         *      cursor. If the position is null, the view should zoom to center
         *      of view widget.
         * @return Whether the zoom level is available
         *
         * Subclasses should emit zoomChanged() signal on success.
         */
        virtual bool zoomTo(Core::Zoom zoom, const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Move map to given coordinates
         * @param coords Coordinates
         * @param pos       Position to which apply the coordinates, relative to
         *      map view widget. If the position is null, the coordinates should
         *      be applied to center of view widget.
         * @return Whether the map can be moved to given coordinates (they
         *      are valid and they aren't out of available map area).
         *
         * Moves the map so given coordinates are centered in view area.
         */
        virtual bool setCoords(const Core::LatLonCoords& coords, const QPoint& pos = QPoint()) = 0;

        /**
         * @brief Move map
         * @param x         X length
         * @param y         Y length
         * @return Whether the map can be moved (whether map area is available)
         */
        virtual bool move(int x, int y) = 0;

        /*@}*/

    protected:
        /**
         * @brief Context menu event
         *
         * If current model supports coordinate conversion, on right click
         * displays context menu which allows copying current coordinates to
         * clipboard.
         */
        void contextMenuEvent(QContextMenuEvent* event);

        /**
         * @brief Whether the view is ready for displaying
         *
         * View is ready when has an raster model assigned and the raster model
         * has non-zero area, at least one zoom level and one map layer.
         *
         * @see AbstractRasterModel::isUsable()
         */
        bool isReady();

        TileDataThread* tileDataThread;         /**< @brief Thread for downloading tile data */

    signals:
        /**
         * @brief Coordinates under mouse
         * @param coords    Coordinates
         *
         * These coordinates are displayed in status bar.
         */
        void currentCoordinates(const Core::LatLonCoords& coords);

        /**
         * @brief Zoom level changed
         * @param zoom      Current zoom level
         *
         * @see zoomIn(), zoomOut(), zoomTo()
         */
        void zoomChanged(Core::Zoom zoom);

        /**
         * @brief Layer changed
         * @param layer     Current layer name
         *
         * @see setLayer()
         */
        void layerChanged(const QString& layer);

        /**
         * @brief Overlays changed
         * @param overlays  List of currently loaded overlays
         *
         * @see addOverlay(), removeOverlay()
         */
        void overlaysChanged(const QStringList& overlays);

    protected slots:
        /**
         * @brief Tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         * @param data      Tile data
         */
        virtual void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QByteArray& data) = 0;

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

    private slots:
        void copyCoordsToClipboard();

    private:
        QMenu* contextMenu;
        QAction* coordinatesAction;
        QString lastCoordsForClipboard;
};

}}

#endif
