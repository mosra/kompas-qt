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

#include "AbstractMapView.h"

class QAction;

namespace Map2X {

namespace PluginManager {
    template<class T> class PluginManager;
}

namespace QtGui {

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

    private:
        PluginManager::PluginManager<AbstractMapView>* viewPluginManager;
        PluginManager::PluginManager<AbstractTileModel>* tilePluginManager;

        AbstractMapView* view;
        AbstractTileModel* tileModel;

        QAction *quitAction,
            *aboutQtAction;

        void createActions();
        void createMenus();
};

}}

#endif
