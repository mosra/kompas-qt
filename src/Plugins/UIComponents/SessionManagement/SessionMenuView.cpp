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

#include "SessionMenuView.h"

#include <QtGui/QMenu>

#include "SessionManager.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SessionMenuView::SessionMenuView(SessionManager* _manager, QMenu* _menu, QAction* before, QObject* parent): QObject(parent), manager(_manager), menu(_menu), before(before) {
    group = new QActionGroup(this);

    /* Connect manager name changes */
    connect(manager, SIGNAL(namesChanged()), SLOT(updateNames()));

    /* Connect manager current changes */
    connect(manager, SIGNAL(currentChanged(uint)), SLOT(updateCurrent(uint)));

    /* Connect menu triggers */
    connect(menu, SIGNAL(triggered(QAction*)), SLOT(trigger(QAction*)));

    /* Fill the menu for the first time */
    updateNames();
}

void SessionMenuView::updateNames() {
    QList<QAction*> actions = sessions.keys();
    qDeleteAll(actions.begin(), actions.end());
    sessions.clear();

    QAction* action = new QAction(tr("(default session)"), group);
    if(manager->current() == 0 && manager->isLoaded()) {
        action->setCheckable(true);
        action->setChecked(true);
    }
    menu->insertAction(before, action);
    sessions.insert(action, 0);
    group->addAction(action);

    QStringList names = manager->names();
    for(int i = 0; i != names.size(); ++i) {
        QAction* action = new QAction(names[i], group);
        menu->insertAction(before, action);
        group->addAction(action);
        if(manager->current() == static_cast<unsigned int>(i+1) && manager->isLoaded()) {
            action->setCheckable(true);
            action->setChecked(true);
        }
        sessions.insert(action, i+1);
    }
}

void SessionMenuView::updateCurrent(unsigned int id) {
    QAction* a = group->checkedAction();
    if(a) {
        a->setCheckable(false);
        a->setChecked(false);
    }

    a = sessions.key(id);
    if(a) {
        a->setCheckable(true);
        a->setChecked(true);
    }
}

void SessionMenuView::trigger(QAction* action) {
    if(!sessions.contains(action)) return;

    manager->load(sessions.value(action));
}

}}}
