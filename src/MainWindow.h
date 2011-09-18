#ifndef Kompas_QtGui_MainWindow_h
#define Kompas_QtGui_MainWindow_h
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
 * @brief Class Kompas::QtGui::MainWindow
 */

#include <QtCore/QMultiMap>
#include <QtCore/QReadWriteLock>
#include <QtGui/QMainWindow>

#include "Utility/Configuration.h"
#include "SessionManager.h"
#include "AbstractUIComponent.h"
#include "Locker.h"
#include "AbstractRasterModel.h"

class QStackedWidget;
class QAction;
class QMenu;

namespace Kompas {

namespace Core {
    class AbstractCache;
}

namespace QtGui {

class AbstractMapView;
class PluginManagerStore;
class RasterPackageModel;
class RasterLayerModel;
class RasterOverlayModel;
class RasterZoomModel;

/**
 * @brief %Kompas main window
 * @todo @c VERSION-0.2 Session management
 */
class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        /** @brief Global instance of MainWindow */
        inline static MainWindow* instance() { return _instance; }

        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param flags     Window flags
         */
        MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

        /**
         * @brief Set additional window title
         *
         * If @c title is empty, displays <tt>"Kompas VERSION"</tt>, otherwise
         * displays <tt>"[ title ] - Kompas"</tt>.
         */
        void setWindowTitle(const QString& title);

        /** @brief Global application configuration */
        inline Utility::Configuration* configuration()
            { return &_configuration; }

        /** @brief Session manager */
        inline SessionManager* sessionManager()
            { return _sessionManager; }

        /** @brief Plugin manager store */
        inline PluginManagerStore* pluginManagerStore()
            { return _pluginManagerStore; }

        /** @brief Global raster map package model */
        inline RasterPackageModel* rasterPackageModel()
            { return _rasterPackageModel; }

        /** @brief Global raster map layer model */
        inline RasterLayerModel* rasterLayerModel()
            { return _rasterLayerModel; }

        /** @brief Global raster map layer model */
        inline RasterOverlayModel* rasterOverlayModel()
            { return _rasterOverlayModel; }

        /** @brief Global raster map layer model */
        inline RasterZoomModel* rasterZoomModel()
            { return _rasterZoomModel; }

        /**
         * @brief Get raster model for reading
         * @return Locker with raster model
         *
         * This functions locks raster model for reading. After usage the model
         * has to be unlocked either by destroying @ref Locker instance or
         * calling @ref Locker::unlock().
         */
        inline Locker<const Core::AbstractRasterModel> rasterModelForRead() {
            return Locker<const Core::AbstractRasterModel>(_rasterModel, &rasterModelLock);
        }

        /**
         * @brief Get raster model for writing
         * @return Locker with raster model
         *
         * This functions locks raster model for writing. After usage the model
         * has to be unlocked either by destroying @ref Locker instance or
         * calling @ref Locker::unlock().
         */
        inline Locker<Core::AbstractRasterModel> rasterModelForWrite() {
            return Locker<Core::AbstractRasterModel>(_rasterModel, &rasterModelLock);
        }

        /**
         * @brief Open raster map file
         *
         * Allows user to select an file through open file dialog. First checks
         * whether the file can be opened with currently active model (if it
         * either supports multiple packages or doesn't have any opened
         * packages), otherwise goes through all available raster plugins
         * and tries to open the file with them.
         */
        Core::AbstractRasterModel* rasterModelForFile(const QString& filename, Core::AbstractRasterModel::SupportLevel* supportLevel);

        /**
         * @brief Map view
         * @return Current map view. The pointer should not be stored anywhere
         *      because it can change after loading another map view plugin.
         */
        inline AbstractMapView* mapView() { return _mapView; }

    signals:
        /** @brief Map view has been changed to another */
        void mapViewChanged();

        /**
         * @brief Raster model has been changed
         * @param previous  If set, the model is changed to another. Otherwise
         *      just a new package was loaded or online maps enabled / disabled.
         * @todo @c VERSION-0.2 Emit something else when the model wasn't
         *      changed to another
         */
        void rasterModelChanged(const Core::AbstractRasterModel* previous = 0);

        /**
         * @brief Action has been added
         *
         * All active @ref AbstractUIComponent plugins are connected to this
         * signal.
         */
        void actionAdded(int category, QAction* action);

    public slots:
        /**
         * @brief Load default configuration
         *
         * Goes through configuration and if any group or key is missing, sets
         * it to default value.
         */
        void loadDefaultConfiguration();

        /**
         * @brief Set map view
         * @param view      Instance of map view
         *
         * Replaces current map view with given instance.
         */
        void setMapView(AbstractMapView* view);

        /**
         * @brief Set raster model
         * @param model     Instance of raster model
         *
         * Replaces current raster model with given instance.
         */
        void setRasterModel(Core::AbstractRasterModel* model);

        /**
         * @brief Enable or disable online maps
         * @param enabled   Whether disable or enable
         */
        void setOnlineEnabled(bool enabled);

    private slots:
        /**
         * @brief Load UI component
         * @param plugin    Plugin name
         * @param loadState Load state
         */
        void loadUIComponent(const std::string& plugin, int, int loadState);

    private:
        static MainWindow* _instance;

        Utility::Configuration _configuration;
        SessionManager* _sessionManager;

        PluginManagerStore* _pluginManagerStore;

        RasterPackageModel* _rasterPackageModel;
        RasterLayerModel* _rasterLayerModel;
        RasterOverlayModel* _rasterOverlayModel;
        RasterZoomModel* _rasterZoomModel;

        AbstractMapView* _mapView;
        Core::AbstractCache* _cache;
        Core::AbstractRasterModel* _rasterModel;
        QReadWriteLock rasterModelLock;

        QMultiMap<AbstractUIComponent::ActionCategory, QAction*> _actions;

        QList<QDockWidget*> _dockWidgets;

        void createUI();

        void displayMapIfUsable();
};

}}

#endif
