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
#include "AbstractTileModel.h"
#include "AbstractMapView.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MapOptionsDock::MapOptionsDock(MainWindow* _mainWindow, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), mainWindow(_mainWindow) {
    /* Tile model combobox */
    tileModelsModel = new PluginModel(mainWindow->tileModelPluginManager(), PluginModel::LoadedOnly, this);
    tileModels = new QComboBox;
    tileModels->setModel(tileModelsModel);
    tileModels->setModelColumn(PluginModel::Name);

    /* Tile layers combobox */
    tileLayerModel = new TileLayerModel(this);
    tileLayers = new QComboBox;
    tileLayers->setModel(tileLayerModel);

    /* Tile overlays combobox */
    tileOverlayModel = new TileOverlayModel(this);
    EditableTileOverlayModel* editableTileOverlayModel = new EditableTileOverlayModel(mainWindow->mapView(), this);
    editableTileOverlayModel->setSourceModel(tileOverlayModel);
    tileOverlays = new QListView;
    tileOverlays->setModel(editableTileOverlayModel);

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

void MapOptionsDock::EditableTileOverlayModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
}

QModelIndex MapOptionsDock::EditableTileOverlayModel::mapFromSource(const QModelIndex& sourceIndex) const {
    return index(sourceIndex.row(), sourceIndex.column());
}

QModelIndex MapOptionsDock::EditableTileOverlayModel::mapToSource(const QModelIndex& proxyIndex) const {
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

void MapOptionsDock::EditableTileOverlayModel::reload() {
    beginResetModel();
    loaded.clear();

    if(*mapView) {
        /* Make sure loadedOverlays bitarray is as large as overlays list */
        loaded.fill(false, sourceModel()->rowCount());

        QStringList _loaded = (*mapView)->overlays();
        for(int row = 0; row != sourceModel()->rowCount(); ++row) {
            if(_loaded.contains(sourceModel()->index(row, 0).data().toString()))
                loaded.setBit(row, true);
        }
    }

    endResetModel();
}

QVariant MapOptionsDock::EditableTileOverlayModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::CheckStateRole && index.isValid() && index.column() == 0 && index.row() < rowCount())
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QAbstractProxyModel::data(index, role);
}

Qt::ItemFlags MapOptionsDock::EditableTileOverlayModel::flags(const QModelIndex& index) const {
    if(index.isValid() && index.column() == 0 && index.row() < rowCount())
        return QAbstractProxyModel::flags(index)|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index);
}

bool MapOptionsDock::EditableTileOverlayModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(index.isValid() && index.column() == 0 && index.row() < rowCount() && role == Qt::CheckStateRole) {
        /* Remove overlay */
        if(loaded.at(index.row())) {
            if((*mapView)->removeOverlay(data(index).toString())) {
                loaded.setBit(index.row(), false);
                emit dataChanged(index, index);
                return true;
            }

        /* Add overlay */
        } else {
            if((*mapView)->addOverlay(data(index).toString())) {
                loaded.setBit(index.row(), true);
                emit dataChanged(index, index);
                return true;
            }
        }
    }

    return QAbstractProxyModel::setData(index, value, role);
}

}}
