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

#include "PluginConfigurationUIComponent.h"

#include <QtGui/QAction>

#include "PluginDialog.h"
#include "MainWindow.h"

PLUGIN_REGISTER(PluginConfigurationUIComponent, Kompas::Plugins::UIComponents::PluginConfigurationUIComponent,
                "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace std;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

PluginConfigurationUIComponent::PluginConfigurationUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const string& plugin): AbstractUIComponent(manager, plugin) {
    QAction* action = new QAction(QIcon(":/plugins-16.png"), tr("Plugins"), this);
    connect(action, SIGNAL(triggered(bool)), SLOT(pluginDialog()));
    _actions << action;
}

void PluginConfigurationUIComponent::pluginDialog() {
    PluginDialog* dialog = new PluginDialog(MainWindow::instance());
    dialog->show();
}

}}}
