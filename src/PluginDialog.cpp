/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "PluginDialog.h"

#include <QtGui/QTabWidget>

#include "PluginManager.h"
#include "MainWindow.h"
#include "PluginDialogTab.h"

namespace Map2X { namespace QtGui {

PluginDialog::PluginDialog(MainWindow* mainWindow, QWidget* parent, Qt::WindowFlags f): AbstractConfigurationDialog(parent, f) {
    /* Tabs */
    tabs = new QTabWidget;
    PluginDialogTab* mapViewTab = new PluginDialogTab(
        mainWindow,
        "mapViews",
        mainWindow->mapViewPluginManager(),
        tr("Plugins providing map view area."));
    tabs->addTab(mapViewTab, tr("Map viewers"));
    connectWidget(mapViewTab);

    PluginDialogTab* projectionTab = new PluginDialogTab(
        mainWindow,
        "projections",
        mainWindow->projectionPluginManager(),
        tr("Plugins for map projections."));
    tabs->addTab(projectionTab, tr("Projections"));
    connectWidget(projectionTab);

    PluginDialogTab* tileModelTab = new PluginDialogTab(
        mainWindow,
        "tileModels",
        mainWindow->tileModelPluginManager(),
        tr("Plugins for displaying different kinds of raster maps."));
    tabs->addTab(tileModelTab, tr("Raster maps"));
    connectWidget(tileModelTab);

    PluginDialogTab* toolsTab = new PluginDialogTab(
        mainWindow,
        "tools",
        mainWindow->toolPluginManager(),
        tr("Various utilites for data computing and conversion."));
    tabs->addTab(toolsTab, tr("Tools"));
    connectWidget(toolsTab);

    setCentralWidget(tabs);
    setWindowTitle("Plugins");
    resize(640, 480);
}

}}
