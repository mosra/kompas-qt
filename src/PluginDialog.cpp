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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>

#include "PluginManager/PluginManager.h"
#include "MainWindow.h"
#include "PluginModel.h"
#include "PluginDialogTab.h"

namespace Map2X { namespace QtGui {

PluginDialog::PluginDialog(MainWindow* mainWindow, QWidget* parent, Qt::WindowFlags f): QDialog(parent, f) {
    /* Tab area */
    QTabWidget* tabs = new QTabWidget;
    tabs->addTab(new PluginDialogTab(
        mainWindow->mapViewPluginManager(),
        tr("Plugins providing map view area.")),
        tr("Map viewers")
    );
    tabs->addTab(new PluginDialogTab(
        mainWindow->tileModelPluginManager(),
        tr("Plugins for displaying different kinds of raster maps.")),
        tr("Raster maps")
    );

    /* Buttons */
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));

    /* Layout */
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(tabs);
    layout->addWidget(buttons);
    setLayout(layout);

    setWindowTitle("Plugins");
    resize(640, 320);
}

}}
