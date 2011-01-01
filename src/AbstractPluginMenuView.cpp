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

#include "AbstractPluginMenuView.h"

using namespace std;

namespace Kompas { namespace QtGui {

AbstractPluginMenuView::AbstractPluginMenuView(AbstractPluginManager* _manager, QMenu* _menu, QAction* _before, QObject* parent): QObject(parent), manager(_manager), menu(_menu), before(_before) {
    /* Update menu after plugin changes */
    connect(_manager, SIGNAL(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)), this, SLOT(tryUpdate(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));
    connect(_manager, SIGNAL(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
    this, SLOT(tryUpdate(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));

    /* Trigger an action after clicking the menu */
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(trigger(QAction*)));
}

void AbstractPluginMenuView::tryUpdate(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    /* State changed between loaded <-> unloaded => update menu. Static plugins
        aren't changing, so don't test them. */
    if((before & AbstractPluginManager::LoadOk) != (after & AbstractPluginManager::LoadOk))
        update();
}

void AbstractPluginMenuView::update() {
    /* Clear menu before adding new items */
    clearMenu();

    /* Foreach plugin list and create menu items */
    vector<string> plugins = manager->pluginList();

    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(manager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
            continue;

        if(QAction* action = createMenuAction(*it))
            menu->insertAction(before, action);
    }
}

}}
