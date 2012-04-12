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

#include "MobileUIComponent.h"

#include "MainWindow.h"
#include "PluginManagerStore.h"

PLUGIN_REGISTER(MobileUIComponent, Kompas::Plugins::UIComponents::MobileUIComponent,
                "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MobileUIComponent::MobileUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    AbstractRasterModel* rasterModel = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager()->instance("OpenStreetMapRasterModel");
    rasterModel->setOnline(true);
    MainWindow::instance()->setRasterModel(rasterModel);
    MainWindow::instance()->resize(640, 360);

    _centralWidget = new MobileCentralWidget;
}

}}}
