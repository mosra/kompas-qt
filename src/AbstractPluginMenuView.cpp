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

#include "AbstractPluginMenuView.h"

using namespace std;

namespace Map2X { namespace QtGui {

void AbstractPluginMenuView::update() {
    /* Foreach plugin list and create menu items */
    vector<string> plugins = manager->nameList();

    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(manager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
            continue;

        if(QAction* action = createMenuAction(*it))
            menu->insertAction(before, action);
    }

    /* Trigger an action after clicking the menu */
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(trigger(QAction*)));
}

}}
