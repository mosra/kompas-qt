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

#include "ConfigurationUIComponent.h"

#include <QtGui/QAction>

#include "ConfigurationDialog.h"

PLUGIN_REGISTER_STATIC(ConfigurationUIComponent,
                       Kompas::Plugins::UIComponents::ConfigurationUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

namespace Kompas { namespace Plugins { namespace UIComponents {

ConfigurationUIComponent::ConfigurationUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    QAction* action = new QAction(QIcon(":/settings-16.png"), tr("Configure Kompas"), this);
    connect(action, SIGNAL(triggered(bool)), SLOT(configurationDialog()));
    _actions << action;
}

void ConfigurationUIComponent::configurationDialog() {
    ConfigurationDialog dialog;
    dialog.exec();
}

}}}
