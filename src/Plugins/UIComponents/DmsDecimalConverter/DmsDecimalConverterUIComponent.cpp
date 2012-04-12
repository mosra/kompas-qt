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

#include "DmsDecimalConverterUIComponent.h"

#include <QtGui/QAction>

#include "MainWindow.h"
#include "DmsDecimalConverterDialog.h"

PLUGIN_REGISTER(DmsDecimalConverterUIComponent, Kompas::Plugins::UIComponents::DmsDecimalConverterUIComponent,
                "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace std;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

DmsDecimalConverterUIComponent::DmsDecimalConverterUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const string& plugin): AbstractUIComponent(manager, plugin) {
    QAction* action = new QAction(QString::fromStdString(*metadata()->name()), this);
    connect(action, SIGNAL(triggered(bool)), SLOT(dmsDecimalConverterDialog()));
    _actions << action;
}

void DmsDecimalConverterUIComponent::dmsDecimalConverterDialog() {
    DmsDecimalConverterDialog* dialog = new DmsDecimalConverterDialog(MainWindow::instance());
    dialog->setWindowTitle(QString::fromStdString(*metadata()->name()));
    dialog->show();
}

}}}
