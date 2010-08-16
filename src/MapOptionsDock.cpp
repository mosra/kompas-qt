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

#include "MapOptionsDock.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>

#include "PluginManager/PluginManager.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "TileLayerModel.h"
#include "TileOverlayModel.h"

namespace Map2X { namespace QtGui {

MapOptionsDock::MapOptionsDock(MainWindow* _mainWindow, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    /* Tile model combobox */
    tileModels = new QComboBox;
    tileModels->setModel(new PluginModel(_mainWindow->tileModelPluginManager(), PluginModel::LoadedOnly, this));
    tileModels->setModelColumn(PluginModel::Name);

    /* Tile layers combobox */
    tileLayers = new QComboBox;
    tileLayers->setModel(new TileLayerModel(_mainWindow->tileModel(), this));

    /* Tile overlays combobox */
    tileOverlays = new QListView;
    tileOverlays->setModel(new TileOverlayModel(_mainWindow->tileModel(), _mainWindow->mapView(), 0, this));

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Server:")), 0, 0);
    layout->addWidget(tileModels, 0, 1);
    layout->addWidget(new QLabel(tr("Map layer:")), 5, 0);
    layout->addWidget(tileLayers, 5, 1);
    layout->addWidget(new QLabel(tr("Overlays:")), 6, 0, 1, 2);
    layout->addWidget(tileOverlays, 7, 0, 1, 2);
    layout->addWidget(new QWidget, 8, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(8, 1);
    setLayout(layout);
}

}}
