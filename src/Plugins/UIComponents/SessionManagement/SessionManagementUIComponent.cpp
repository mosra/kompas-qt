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

#include "SessionManagementUIComponent.h"

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QInputDialog>

#include "Utility/Directory.h"
#include "MainWindow.h"
#include "MessageBox.h"
#include "SessionManagerDialog.h"
#include "SessionMenuView.h"

PLUGIN_REGISTER(SessionManagementUIComponent, Kompas::Plugins::UIComponents::SessionManagementUIComponent,
                "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SessionManagementUIComponent::SessionManagementUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* Open session */
    QIcon openSessionIcon;
    openSessionIcon.addFile(":/open-session-16.png");
    openSessionIcon.addFile(":/open-session-64.png");
    QAction* openSessionAction = new QAction(openSessionIcon, tr("Restore saved session"), this);
    openSessionAction->setData("openSession");

    _actions << openSessionAction;

    /* Session list menu */
    sessionMenu = new QMenu(MainWindow::instance());
    openSessionAction->setMenu(sessionMenu);
    QAction* separator = sessionMenu->addSeparator();
    sessionMenu->addAction(tr("Session manager"), this, SLOT(sessionManagerDialog()));
    new SessionMenuView(MainWindow::instance()->sessionManager(), sessionMenu, separator, this);
}

void SessionManagementUIComponent::sessionManagerDialog() {
    SessionManagerDialog* dialog = new SessionManagerDialog(MainWindow::instance());
    dialog->show();
}

}}}
