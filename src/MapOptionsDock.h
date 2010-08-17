#ifndef Map2X_QtGui_MapOptionsDock_h
#define Map2X_QtGui_MapOptionsDock_h
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
 * @brief Class Map2X::QtGui::MapOptionsDock
 */

#include <QtGui/QDockWidget>

class QListView;
class QComboBox;

namespace Map2X { namespace QtGui {

class MainWindow;
class PluginModel;
class TileOverlayModel;
class TileLayerModel;

/** @brief Dock widget with map options */
class MapOptionsDock: public QWidget {
    public:
        /**
         * @brief Constructor
         * @param _mainWindow       Main window instance
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        MapOptionsDock(MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    private:
        MainWindow* mainWindow;

        QComboBox *tileModels,
            *tileLayers;
        QListView* tileOverlays;

        PluginModel* tileModelsModel;
        TileLayerModel* tileLayerModel;
        TileOverlayModel* tileOverlayModel;
};

}}

#endif
