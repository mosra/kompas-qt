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
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "AbstractRasterModel.h"
#include "AbstractMapView.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MapOptionsDock::MapOptionsDock(MainWindow* _mainWindow, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), mainWindow(_mainWindow) {
    /* Tile model combobox */
    rasterModelsModel = new PluginModel(mainWindow->rasterModelPluginManager(), PluginModel::LoadedOnly, this);
    rasterModels = new QComboBox;
    rasterModels->setModel(rasterModelsModel);
    rasterModels->setModelColumn(PluginModel::Name);

    /* Tile layers combobox */
    rasterLayerModel = new RasterLayerModel(this);
    rasterLayers = new QComboBox;
    rasterLayers->setModel(rasterLayerModel);

    /* Tile overlays combobox */
    rasterOverlayModel = new RasterOverlayModel(this);
    EditableRasterOverlayModel* editableRasterOverlayModel = new EditableRasterOverlayModel(mainWindow->mapView(), this);
    editableRasterOverlayModel->setSourceModel(rasterOverlayModel);
    rasterOverlays = new QListView;
    rasterOverlays->setModel(editableRasterOverlayModel);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Server:")), 0, 0);
    layout->addWidget(rasterModels, 0, 1);
    layout->addWidget(new QLabel(tr("Map layer:")), 5, 0);
    layout->addWidget(rasterLayers, 5, 1);
    layout->addWidget(new QLabel(tr("Overlays:")), 6, 0, 1, 2);
    layout->addWidget(rasterOverlays, 7, 0, 1, 2);
    layout->addWidget(new QWidget, 8, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(8, 1);
    setLayout(layout);

    /* Set actual tile model */
    rasterModels->setCurrentIndex(rasterModelsModel->findPlugin(QString::fromStdString(mainWindow->configuration()->group("map")->value<string>("rasterModel"))));

    setActualData();

    /* Connect comboboxes with model / layer changing */
    connect(rasterModels, SIGNAL(currentIndexChanged(int)), SLOT(setRasterModel(int)));
    connect(rasterLayers, SIGNAL(currentIndexChanged(QString)), *mainWindow->mapView(), SLOT(setLayer(QString)));
}

void MapOptionsDock::setRasterModel(int number) {
    mainWindow->setRasterModel(rasterModelsModel->index(number, PluginModel::Plugin).data().toString());
    rasterLayerModel->reload();
    rasterOverlayModel->reload();
    setActualData();
}

void MapOptionsDock::setActualData() {
    /* Set actual map layer */
    rasterLayers->setCurrentIndex(rasterLayers->findText((*mainWindow->mapView())->layer()));

    /** @todo Actual overlays? */
}

void MapOptionsDock::EditableRasterOverlayModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
}

QModelIndex MapOptionsDock::EditableRasterOverlayModel::mapFromSource(const QModelIndex& sourceIndex) const {
    return index(sourceIndex.row(), sourceIndex.column());
}

QModelIndex MapOptionsDock::EditableRasterOverlayModel::mapToSource(const QModelIndex& proxyIndex) const {
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

void MapOptionsDock::EditableRasterOverlayModel::reload() {
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

QVariant MapOptionsDock::EditableRasterOverlayModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::CheckStateRole && index.isValid() && index.column() == 0 && index.row() < rowCount())
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QAbstractProxyModel::data(index, role);
}

Qt::ItemFlags MapOptionsDock::EditableRasterOverlayModel::flags(const QModelIndex& index) const {
    if(index.isValid() && index.column() == 0 && index.row() < rowCount())
        return QAbstractProxyModel::flags(index)|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index);
}

bool MapOptionsDock::EditableRasterOverlayModel::setData(const QModelIndex& index, const QVariant& value, int role) {
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
