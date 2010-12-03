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

#include <QtGui/QAction>

#include "PluginManager.h"

using namespace std;

namespace Map2X { namespace QtGui {

void ToolPluginMenuView::trigger(QAction* action) {
    /* Instantiate dialog */
    AbstractToolDialog* dialog = items.value(action)->create(mainWindow, mainWindow);
    dialog->show();
}

void ToolPluginMenuView::clearMenu() {
    qDeleteAll<QList<QAction*> >(items.keys());
    qDeleteAll<QList<AbstractTool*> >(items.values());
    items.clear();
}

QAction* ToolPluginMenuView::createMenuAction(const string& pluginName) {
    AbstractTool* instance = toolManager->instance(pluginName);
    QAction* action = new QAction(instance->menuIcon(), instance->menuText(), this);
    items.insert(action, instance);

    return action;
}

}}
