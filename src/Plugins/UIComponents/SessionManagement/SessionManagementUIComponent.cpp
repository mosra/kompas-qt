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
#include "SessionMenuView.h"

PLUGIN_REGISTER_STATIC(SessionManagementUIComponent,
                       Kompas::Plugins::UIComponents::SessionManagementUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace Kompas::Utility;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SessionManagementUIComponent::SessionManagementUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* Open session */
    QIcon openSessionIcon;
    openSessionIcon.addFile(":/open-session-16.png");
    openSessionIcon.addFile(":/open-session-64.png");
    QAction* openSessionAction = new QAction(openSessionIcon, tr("Restore saved session"), this);

    /* Create new session */
    QAction* newSessionAction = new QAction(tr("Create new session"), this);
    connect(newSessionAction, SIGNAL(triggered(bool)), SLOT(newSession()));

    /* Rename current session */
    renameSessionAction = new QAction(tr("Rename current session"), this);
    connect(renameSessionAction, SIGNAL(triggered(bool)), SLOT(renameSession()));

    /* Delete current session */
    deleteSessionAction = new QAction(tr("Delete current session"), this);
    connect(deleteSessionAction, SIGNAL(triggered(bool)), SLOT(deleteSession()));

    _actions << openSessionAction << newSessionAction << renameSessionAction << deleteSessionAction;

    /* Session list menu */
    sessionMenu = new QMenu(MainWindow::instance());
    openSessionAction->setMenu(sessionMenu);
    sessionMenu->addAction(newSessionAction);
    sessionMenu->addSeparator();
    new SessionMenuView(MainWindow::instance()->sessionManager(), sessionMenu, this);

    /* Sessions */
    currentSessionChange();
    connect(MainWindow::instance()->sessionManager(), SIGNAL(currentChanged(uint)), SLOT(currentSessionChange()));
}

void SessionManagementUIComponent::currentSessionChange() {
    /* Window title, disable/enable menu items */
    if(!MainWindow::instance()->sessionManager()->isLoaded() || MainWindow::instance()->sessionManager()->current() == 0) {
        MainWindow::instance()->setWindowTitle("");
        renameSessionAction->setDisabled(true);
        deleteSessionAction->setDisabled(true);
    } else {
        MainWindow::instance()->setWindowTitle(MainWindow::instance()->sessionManager()->names()[MainWindow::instance()->sessionManager()->current()-1]);
        renameSessionAction->setDisabled(false);
        deleteSessionAction->setDisabled(false);
    }
}

void SessionManagementUIComponent::newSession() {
    bool ok;
    QString name = QInputDialog::getText(MainWindow::instance(), tr("New session"), tr("Enter new session name:"), QLineEdit::Normal, tr("New session"), &ok);
    if(!ok) return;

    unsigned int id = MainWindow::instance()->sessionManager()->newSession(name);
    MainWindow::instance()->sessionManager()->load(id);
}

void SessionManagementUIComponent::renameSession() {
    if(MainWindow::instance()->sessionManager()->current() == 0) return;

    bool ok;
    QString name = QInputDialog::getText(MainWindow::instance(), tr("Rename session"), tr("Enter new session name:"), QLineEdit::Normal, MainWindow::instance()->sessionManager()->names()[MainWindow::instance()->sessionManager()->current()-1], &ok);
    if(!ok) return;

    MainWindow::instance()->sessionManager()->renameSession(MainWindow::instance()->sessionManager()->current(), name);
}

void SessionManagementUIComponent::deleteSession() {
    if(MainWindow::instance()->sessionManager()->current() == 0) return;

    if(MessageBox::question(MainWindow::instance(), tr("Delete session"), tr("Are you sure you want to delete session '%0'?").arg(MainWindow::instance()->sessionManager()->names()[MainWindow::instance()->sessionManager()->current()-1]), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;

    MainWindow::instance()->sessionManager()->deleteSession(MainWindow::instance()->sessionManager()->current());
    MainWindow::instance()->sessionManager()->load(0);
}

}}}
