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

#include "StatusBarUIComponent.h"

#include <QtGui/QStatusBar>
#include <QtGui/QLabel>

#include "MainWindow.h"

using namespace Kompas::Core;
using namespace Kompas::QtGui;

PLUGIN_REGISTER_STATIC(StatusBarUIComponent,
                       Kompas::Plugins::UIComponents::StatusBarUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

namespace Kompas { namespace Plugins { namespace UIComponents {

StatusBarUIComponent::StatusBarUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    _coordinateStatus = new QLabel;
    _statusBar = new QStatusBar(MainWindow::instance());
    _statusBar->addPermanentWidget(_coordinateStatus);

    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)), this, SLOT(rasterModelChanged(const Core::AbstractRasterModel*)));
    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(mapViewChanged()));
    mapViewChanged();
}

void StatusBarUIComponent::mapViewChanged() {
    if(MainWindow::instance()->mapView())
        connect(MainWindow::instance()->mapView(), SIGNAL(currentCoordinates(Core::LatLonCoords)), SLOT(currentCoordinates(Core::LatLonCoords)));
}

void StatusBarUIComponent::rasterModelChanged(const AbstractRasterModel* previous) {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    rasterModel.unlock();

    if(MainWindow::instance()->mapView() && isUsable)
        _coordinateStatus->setHidden(false);
    else
        _coordinateStatus->setHidden(true);
}

void StatusBarUIComponent::currentCoordinates(const LatLonCoords& coords) {
    if(!coords.isValid())
        _coordinateStatus->setText("");
    else
        _coordinateStatus->setText(QString::fromStdString(coords.toString(0, true)));
}

}}}
