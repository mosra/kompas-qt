#ifndef Kompas_QtGui_ToolPluginMenuView_h
#define Kompas_QtGui_ToolPluginMenuView_h
/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Kompas::QtGui::ToolPluginMenuView
 */

#include <QtCore/QHash>

#include "AbstractPluginMenuView.h"
#include "PluginManager.h"
#include "MainWindow.h"
#include "AbstractTool.h"

class QMenu;
class QAction;

namespace Kompas { namespace QtGui {

template<class T> class PluginManager;

/**
 * @brief Menu view of tool plugins
 *
 * Fills up given menu with loaded tool plugins.
 */
class ToolPluginMenuView: public AbstractPluginMenuView {
    public:
        /**
         * @brief Constructor
         * @param _mainWindow   Pointer to main window
         * @param manager       Plugin manager
         * @param menu          Menu to which add items
         * @param before        Menu item before which add items. If set to 0,
         *      the items will be put at the end of menu.
         * @param parent        Parent widget
         */
        ToolPluginMenuView(MainWindow* _mainWindow, PluginManager<AbstractTool>* manager, QMenu* menu, QAction* before = 0, QObject* parent = 0): AbstractPluginMenuView(manager, menu, before, parent), toolManager(manager), mainWindow(_mainWindow) {}

        inline virtual ~ToolPluginMenuView() { qDeleteAll<QList<AbstractTool*> >(items.values()); }

    private slots:
        virtual void trigger(QAction* action);

    private:
        virtual void clearMenu();
        virtual QAction* createMenuAction(const std::string& pluginName);

        PluginManager<AbstractTool>* toolManager;
        MainWindow* mainWindow;
        QHash<QAction*, AbstractTool*> items;
};

}}

#endif
