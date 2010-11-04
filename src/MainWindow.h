#ifndef Map2X_QtGui_MainWindow_h
#define Map2X_QtGui_MainWindow_h
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
 * @brief Class Map2X::QtGui::MainWindow
 */

#include <QtCore/QReadWriteLock>
#include <QtGui/QMainWindow>

#include "Utility/Configuration.h"
#include "AbstractMapView.h"
#include "AbstractTool.h"
#include "AbstractProjection.h"

class QAction;
class QMenu;
class QLabel;

namespace Map2X { namespace QtGui {

class AbstractPluginManager;
template<class T> class PluginManager;

/** @brief %Map2X main window */
class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
        static MainWindow* instance() { return _instance; }

        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param flags     Window flags
         */
        MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

        /**
         * @brief Destructor
         *
         * Destroys all plugin managers.
         */
        virtual ~MainWindow();

        /** @brief Global application configuration */
        inline Utility::Configuration* configuration()
            { return &_configuration; }

        /** @brief Instance of PluginManager for map view plugins */
        inline PluginManager<AbstractMapView>* mapViewPluginManager()
            { return _mapViewPluginManager; }

        /** @brief Instance of PluginManager for projection plugins */
        inline PluginManager<Core::AbstractProjection>* projectionPluginManager()
            { return _projectionPluginManager; }

        /** @brief Instance of PluginManager for tile model plugins */
        inline PluginManager<Core::AbstractRasterModel>* rasterModelPluginManager()
            { return _rasterModelPluginManager; }

        /** @brief Instance of PluginManager for tool plugins */
        inline PluginManager<AbstractTool>* toolPluginManager()
            { return _toolPluginManager; }

        /**
         * @brief Get tile model for reading
         * @return Pointer to tile model
         *
         * This functions locks tile model for reading. After usage the model
         * have to be unlocked with unlockRasterModel().
         */
        inline const Core::AbstractRasterModel* lockRasterModelForRead() {
            rasterModelLock.lockForRead();
            return _rasterModel;
        }

        /**
         * @brief Get tile model for writing
         * @return Pointer to tile model
         *
         * This functions locks tile model for writing. After usage the model
         * have to be unlocked with unlockRasterModel().
         */
        inline Core::AbstractRasterModel* lockRasterModelForWrite() {
            rasterModelLock.lockForWrite();
            return _rasterModel;
        }

        /**
         * @brief Unlock tile model
         *
         * Unlocks tile model previously locked with lockRasterModelRead() or
         * lockRasterModelWrite().
         */
        inline void unlockRasterModel() {
            rasterModelLock.unlock();
        }

        /**
         * @brief Map view
         * @return Returns double pointer, because the map view class instance
         * can be replaced with another.
         */
        inline AbstractMapView** mapView()
            { return &_mapView; }

    public slots:
        /**
         * @brief Load default configuration
         *
         * Goes through configuration and if any group or key is missing, sets
         * it to default value.
         */
        void loadDefaultConfiguration();

        /**
         * @brief Set tile model
         * @param name      Name of the tile model plugin
         *
         * Sets given tile model.
         */
        void setRasterModel(const QString& name);

    private:
        static MainWindow* _instance;

        Utility::Configuration _configuration;

        PluginManager<AbstractMapView>* _mapViewPluginManager;
        PluginManager<Core::AbstractProjection>* _projectionPluginManager;
        PluginManager<Core::AbstractRasterModel>* _rasterModelPluginManager;
        PluginManager<AbstractTool>* _toolPluginManager;

        AbstractMapView* _mapView;
        Core::AbstractRasterModel* _rasterModel;
        QReadWriteLock rasterModelLock;

        QAction *saveRasterAction,
            *quitAction,
            *zoomInAction,
            *zoomOutAction,
            *pluginDialogAction,
            *configurationDialogAction,
            *aboutQtAction;

        QMenu *saveRasterMenu,
            *toolsMenu;

        QLabel* coordinateStatus;

        void createActions();
        void createMenus();

        void loadPluginsAsConfigured(const std::string& group, AbstractPluginManager* manager);

    private slots:
        void moveMap();
        void pluginDialog();
        void configurationDialog();
        void currentCoordinates(const Core::Wgs84Coords& coords);
};

}}

#endif
