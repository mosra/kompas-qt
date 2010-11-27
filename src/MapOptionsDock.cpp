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
#include "RasterPackageModel.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "AbstractRasterModel.h"
#include "AbstractMapView.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MapOptionsDock::MapOptionsDock(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    mapView = new QComboBox;
    mapView->setModel(new PluginModel(
        MainWindow::instance()->mapViewPluginManager(), PluginModel::LoadedOnly, this));
    mapView->setModelColumn(PluginModel::Name);
    connect(mapView, SIGNAL(currentIndexChanged(int)), SLOT(setMapView(int)));

    /* Raster packages list view */
    EditableRasterPackageModel* editableRasterPackageModel = new EditableRasterPackageModel(this);
    editableRasterPackageModel->setSourceModel(MainWindow::instance()->rasterPackageModel());
    rasterPackages = new QListView;
    rasterPackages->setModel(editableRasterPackageModel);

    /* Raster layers combobox */
    rasterLayers = new QComboBox;
    rasterLayers->setModel(MainWindow::instance()->rasterLayerModel());
    rasterLayers->setModelColumn(RasterPackageModel::Name);

    /* Raster overlays list view */
    EditableRasterOverlayModel* editableRasterOverlayModel = new EditableRasterOverlayModel(MainWindow::instance()->mapView(), this);
    editableRasterOverlayModel->setSourceModel(MainWindow::instance()->rasterOverlayModel());
    rasterOverlays = new QListView;
    rasterOverlays->setModel(editableRasterOverlayModel);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Map view:")), 0, 0);
    layout->addWidget(mapView, 0, 1);
    layout->addWidget(new QLabel(tr("Maps:")), 1, 0, 1, 2);
    layout->addWidget(rasterPackages, 2, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Map layer:")), 3, 0);
    layout->addWidget(rasterLayers, 3, 1);
    layout->addWidget(new QLabel(tr("Overlays:")), 4, 0, 1, 2);
    layout->addWidget(rasterOverlays, 5, 0, 1, 2);
    layout->addWidget(new QWidget, 6, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(6, 1);
    setLayout(layout);

    setActualData();

    /* Update data when raster model is changed */
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged()), SLOT(setActualData()));

    /* Connect comboboxes with model / layer changing */
    /** @todo Make it non-dependent on one map view */
    connect(rasterLayers, SIGNAL(currentIndexChanged(QString)), *MainWindow::instance()->mapView(), SLOT(setLayer(QString)));
}

void MapOptionsDock::setMapView(int id) {
    AbstractMapView* view = MainWindow::instance()->mapViewPluginManager()->instance(mapView->model()->index(id, PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->setMapView(view);
}

void MapOptionsDock::setActualData() {
    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* Raster model is loaded, enable widget */
    if(rasterModel) setDisabled(false);

    /* No raster model loaded, disable widget */
    else setDisabled(true);

    /* Set actual map layer */
    rasterLayers->setCurrentIndex(rasterLayers->findText((*MainWindow::instance()->mapView())->layer()));

    /** @todo Actual overlays? */

    MainWindow::instance()->unlockRasterModel();
}

void MapOptionsDock::EditableRasterPackageModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
}

void MapOptionsDock::EditableRasterPackageModel::reload() {
    beginResetModel();

    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();

    /* If raster model is not available or it doesn't support raster maps, don't show online maps item */
    if(!model || !(model->features() & AbstractRasterModel::LoadableFromUrl)) {
        rasterModelName.clear();
        online = NotSupported;

    /* Else show it and enable/disable it */
    } else {
        rasterModelName = QString::fromStdString(MainWindow::instance()->rasterModelPluginManager()->metadata(model->name())->name());

        if(model->online())     online = Enabled;
        else                    online = Disabled;
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant MapOptionsDock::EditableRasterPackageModel::data(const QModelIndex& _index, int role) const {
    if(online == NotSupported)
        return QAbstractProxyModel::data(_index, role);

    else if(_index.isValid() && _index.row() == 0 &&_index.column() == RasterPackageModel::Name) {
        if(role == Qt::DisplayRole)
            return tr("Online %0").arg(rasterModelName);

        if(role == Qt::CheckStateRole)
            return online == Enabled ? Qt::Checked : Qt::Unchecked;
    }

    return QAbstractProxyModel::data(index(_index.row()-1, _index.column(), _index.parent()), role);
}

Qt::ItemFlags MapOptionsDock::EditableRasterPackageModel::flags(const QModelIndex& _index) const {
    if(online == NotSupported)
        return QAbstractProxyModel::flags(_index);

    else if(online != NotSupported && _index.isValid() && _index.column() == 0 && _index.row() == 0)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index(_index.row()-1, _index.column(), _index.parent()));
}

bool MapOptionsDock::EditableRasterPackageModel::setData(const QModelIndex& _index, const QVariant& value, int role) {
    if(online == NotSupported)
        return QAbstractProxyModel::setData(_index, value, role);

    else if(_index.isValid() && _index.column() == 0 && _index.row() == 0 && role == Qt::CheckStateRole) {
        MainWindow::instance()->setOnlineEnabled(online == Enabled ? false : true);
        return true;
    }

    return QAbstractProxyModel::setData(index(_index.row()-1, _index.column(), _index.parent()), value, role);
}

void MapOptionsDock::EditableRasterOverlayModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
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
