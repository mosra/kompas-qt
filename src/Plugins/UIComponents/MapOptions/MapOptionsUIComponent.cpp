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

#include "MapOptionsUIComponent.h"

#include <QtGui/QDockWidget>

#include "MainWindow.h"
#include "MapOptionsDock.h"

PLUGIN_REGISTER_STATIC(MapOptionsUIComponent,
                       Kompas::Plugins::UIComponents::MapOptionsUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace std;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MapOptionsUIComponent::MapOptionsUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    _dockWidget = new QDockWidget(MainWindow::instance());
    _dockWidget->setWidget(new MapOptionsDock(MainWindow::instance()));
    _dockWidget->setWindowTitle(tr("Map options"));
}

}}}
