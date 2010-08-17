#ifndef Map2X_QtGui_ToolPluginMenuView_h
#define Map2X_QtGui_ToolPluginMenuView_h
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
 * @brief Class Map2X::QtGui::ToolPluginMenuView
 */

#include <QtCore/QObject>
#include <QtCore/QHash>

#include "MainWindow.h"
#include "AbstractTool.h"

class QMenu;
class QAction;

namespace Map2X { namespace QtGui {

template<class T> class PluginManager;

/**
 * @brief Menu view of tool plugins
 *
 * Fills up given menu with loaded tool plugins.
 */
class ToolPluginMenuView: public QObject {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _mainWindow   Pointer to main window
         * @param _manager      Plugin manager with tools
         * @param _menu         Menu to which insert tools
         * @param _before       Menu item before which insert tools
         * @param parent        Parent object
         */
        ToolPluginMenuView(MainWindow* _mainWindow, PluginManager<AbstractTool>* _manager, QMenu* _menu, QAction* _before = 0, QObject* parent = 0);

    public slots:
        /**
         * @brief Update plugin list
         *
         * Reloads menu with actual plugin list.
         */
        void update();

    private slots:
        /** @todo Open as modeless dialog */
        void openToolDialog();

    private:
        MainWindow* mainWindow;
        PluginManager<AbstractTool>* manager;
        QMenu* menu;
        QAction* before;
        QHash<QAction*, AbstractTool*> items;
};

}}

#endif
