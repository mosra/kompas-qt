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

#include "ToolPluginMenuView.h"

#include <QtGui/QMenu>
#include <QtGui/QAction>

#include "PluginManager.h"

using namespace std;

namespace Map2X { namespace QtGui {

ToolPluginMenuView::ToolPluginMenuView(MainWindow* _mainWindow, PluginManager<AbstractTool>* _manager, QMenu* _menu, QAction* _before, QObject* parent): QObject(parent), mainWindow(_mainWindow), manager(_manager), menu(_menu), before(_before) { update(); }

void ToolPluginMenuView::update() {
    if(!items.isEmpty()) {
        qDeleteAll<QList<QAction*> >(items.keys());
        items.clear();
    }

    /* Foreach plugin list and create menu items */
    vector<string> plugins = manager->nameList();

    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(manager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
            continue;

        AbstractTool* instance = manager->instance(*it);
        QAction* action = new QAction(instance->menuIcon(), instance->menuText(), mainWindow);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(openToolDialog()));

        menu->insertAction(before, action);
        items.insert(action, instance);
    }
}

void ToolPluginMenuView::openToolDialog() {
    /* Instantiate dialog */
    AbstractToolDialog* dialog = items.value(qobject_cast<QAction*>(sender()))->create(mainWindow, mainWindow);
    dialog->exec();
    delete dialog;
}

}}
