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

#include "AboutUIComponent.h"

#include <QtGui/QApplication>
#include <QtGui/QAction>

#include "MainWindow.h"
#include "AboutDialog.h"

PLUGIN_REGISTER_STATIC(AboutUIComponent,
                       Kompas::Plugins::UIComponents::AboutUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

AboutUIComponent::AboutUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* About */
    QAction* aboutAction = new QAction(QIcon(":/logo-16.png"), tr("About Kompas"), this);
    aboutAction->setStatusTip(tr("Show information about this application"));
    connect(aboutAction, SIGNAL(triggered(bool)), SLOT(aboutDialog()));
    _actions << aboutAction;

    /* About Qt */
    QAction* aboutQtAction = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    connect(aboutQtAction, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    _actions << aboutQtAction;
}

void AboutUIComponent::aboutDialog() {
    AboutDialog* dialog = new AboutDialog(MainWindow::instance());
    dialog->show();
}

}}}
