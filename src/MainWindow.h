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
#include <QtGui/QFrame>

#include "Utility/Configuration.h"
#include "SessionManager.h"
#include "PluginManagerStore.h"
#include "Locker.h"

class QStackedWidget;
class QAction;
class QMenu;
class QLabel;
class QDockWidget;

namespace Kompas { namespace QtGui {

class RasterPackageModel;
class RasterLayerModel;
class RasterOverlayModel;
class RasterZoomModel;
class SaveRasterMenuView;

/**
 * @brief %Kompas main window
 * @todo @c VERSION-0.2 Session management
 */
class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        /** @brief Global instance of MainWindow */
        static MainWindow* instance() { return _instance; }

        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param flags     Window flags
         */
        MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

        /** @brief Destructor */
        virtual ~MainWindow();

        /** @brief Global application configuration */
        inline Utility::Configuration* configuration()
            { return &_configuration; }

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
         * @brief Open raster map file
         *
         * Allows user to select an file through open file dialog. First checks
         * whether the file can be opened with currently active model (if it
         * either supports multiple packages or doesn't have any opened
         * packages), otherwise goes through all available raster plugins
         * and tries to open the file with them.
         */
        void openRaster();

        /**
         * @brief Save offline package of current raster map
         */
        void saveRaster();

        /**
         * @brief Close raster map
         */
        inline void closeRaster() { setRasterModel(0); }

        /**
         * @brief Reflect current session change in the UI
         *
         * Changes window title and disables rename/delete items in
         * session menu if default session is loaded.
         */
        void currentSessionChange();

        /**
         * @brief Create new session
         *
         * Shows dialog asking for session name, creates new session and
         * switches to it.
         */
        void newSession();

        /**
         * @brief Rename current session
         *
         * Shows dialog asking for session name. If current session is default
         * session, does nothing.
         */
        void renameSession();

        /**
         * @brief Delete session
         *
         * Ask whether to delete, deletes current session and switches to
         * default session. If current session is default session, does nothing.
         */
        void deleteSession();

    private:
        static MainWindow* _instance;

        Utility::Configuration _configuration;
        SessionManager sessionManager;

        PluginManagerStore* _pluginManagerStore;

        RasterPackageModel* _rasterPackageModel;
        RasterLayerModel* _rasterLayerModel;
        RasterOverlayModel* _rasterOverlayModel;
        RasterZoomModel* _rasterZoomModel;

        AbstractMapView* _mapView;
        Core::AbstractRasterModel* _rasterModel;
        QReadWriteLock rasterModelLock;

        QMultiMap<AbstractUIComponent::ActionCategory, QAction*> _actions;

        QAction *openSessionAction,
            *newSessionAction,
            *deleteSessionAction,
            *renameSessionAction,
            *openRasterAction,
            *openOnlineAction,
            *saveRasterAction,
            *saveCurrentRasterAction,
            *closeRasterAction;

        QMenu *sessionMenu,
            *openRasterMenu,
            *saveRasterMenu;

        SaveRasterMenuView* saveRasterMenuView;

        QLabel* coordinateStatus;

        QStackedWidget* centralStackedWidget;
        QDockWidget* mapOptionsDock;

        QList<QDockWidget*> _dockWidgets;

        void createActions();
        void createMenus();
        void createUI();

        void displayMapIfUsable();

    private slots:
        void currentCoordinates(const Core::LatLonCoords& coords);
};

}}

#endif
