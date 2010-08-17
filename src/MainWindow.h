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

#include <QtGui/QMainWindow>

#include "Utility/Configuration.h"
#include "AbstractMapView.h"
#include "AbstractTool.h"

class QAction;
class QMenu;

namespace Map2X { namespace QtGui {

template<class T> class PluginManager;

/** @brief %Map2X main window */
class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
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

        /** @brief Instance of PluginManager for tile model plugins */
        inline PluginManager<Core::AbstractTileModel>* tileModelPluginManager()
            { return _tileModelPluginManager; }

        /** @brief Instance of PluginManager for tool plugins */
        inline PluginManager<AbstractTool>* toolPluginManager()
            { return _toolPluginManager; }

        /**
         * @brief Tile model
         * @return Returns double pointer, because the tile model class instance
         * can be replaced with another.
         */
        inline Core::AbstractTileModel** tileModel()
            { return &_tileModel; }

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

    private:
        Utility::Configuration _configuration;

        PluginManager<AbstractMapView>* _mapViewPluginManager;
        PluginManager<Core::AbstractTileModel>* _tileModelPluginManager;
        PluginManager<AbstractTool>* _toolPluginManager;

        AbstractMapView* _mapView;
        Core::AbstractTileModel* _tileModel;

        QAction *quitAction,
            *moveMapAction,
            *zoomInAction,
            *zoomOutAction,
            *pluginDialogAction,
            *configurationDialogAction,
            *aboutQtAction;

        QMenu* toolsMenu;

        void createActions();
        void createMenus();

    private slots:
        void moveMap();
        void pluginDialog();
        void configurationDialog();
};

}}

#endif
