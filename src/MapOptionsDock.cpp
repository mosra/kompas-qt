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

#include <string>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>

#include "PluginManager.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "TileLayerModel.h"
#include "TileOverlayModel.h"

using namespace std;

namespace Map2X { namespace QtGui {

MapOptionsDock::MapOptionsDock(MainWindow* _mainWindow, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), mainWindow(_mainWindow) {
    /* Tile model combobox */
    tileModelsModel = new PluginModel(mainWindow->tileModelPluginManager(), PluginModel::LoadedOnly, this);
    tileModels = new QComboBox;
    tileModels->setModel(tileModelsModel);
    tileModels->setModelColumn(PluginModel::Name);

    /* Tile layers combobox */
    tileLayerModel = new TileLayerModel(mainWindow->tileModel(), this);
    tileLayers = new QComboBox;
    tileLayers->setModel(tileLayerModel);

    /* Tile overlays combobox */
    tileOverlayModel = new TileOverlayModel(mainWindow->tileModel(), mainWindow->mapView(), 0, this);
    tileOverlays = new QListView;
    tileOverlays->setModel(tileOverlayModel);

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

    /* Set actual tile model */
    tileModels->setCurrentIndex(tileModelsModel->findPlugin(QString::fromStdString(mainWindow->configuration()->group("map")->value<string>("tileModel"))));

    setActualData();

    /* Connect comboboxes with model / layer changing */
    connect(tileModels, SIGNAL(currentIndexChanged(int)), SLOT(setTileModel(int)));
    connect(tileLayers, SIGNAL(currentIndexChanged(QString)), *mainWindow->mapView(), SLOT(setLayer(QString)));
}

void MapOptionsDock::setTileModel(int number) {
    mainWindow->setTileModel(tileModelsModel->index(number, PluginModel::Plugin).data().toString());
    tileLayerModel->reload();
    tileOverlayModel->reload();
    setActualData();
}

void MapOptionsDock::setActualData() {
    /* Set actual map layer */
    tileLayers->setCurrentIndex(tileLayers->findText((*mainWindow->mapView())->layer()));

    /** @todo Actual overlays? */
}

}}
