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
#include "MovingWidget.h"
#include "ZoomSlider.h"

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

MapOptionsDock::MapOptionsDock(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    PluginModel* mapViewModel = new PluginModel(
        MainWindow::instance()->mapViewPluginManager(), PluginModel::LoadedOnly, this);
    mapView = new QComboBox;
    mapView->setModel(mapViewModel);
    mapView->setModelColumn(PluginModel::Name);
    mapView->setCurrentIndex(mapViewModel->findPlugin(QString::fromStdString(
        MainWindow::instance()->configuration()->group("map")->value<string>("viewPlugin"))));
    connect(mapView, SIGNAL(currentIndexChanged(int)), SLOT(setMapView(int)));

    /* Raster packages list view */
    EditableRasterPackageModel* editableRasterPackageModel = new EditableRasterPackageModel(this);
    editableRasterPackageModel->setSourceModel(MainWindow::instance()->rasterPackageModel());
    rasterPackages = new QListView;
    rasterPackages->setModel(editableRasterPackageModel);
    rasterPackages->setFixedHeight(100);

    /* Raster layers combobox */
    rasterLayers = new QComboBox;
    rasterLayers->setModel(MainWindow::instance()->rasterLayerModel());
    rasterLayers->setModelColumn(RasterLayerModel::Translated);
    connect(rasterLayers, SIGNAL(currentIndexChanged(int)), SLOT(setActualLayer(int)));

    /* Raster overlays list view */
    rasterOverlayModel = new EditableRasterOverlayModel(this);
    rasterOverlayModel->setSourceModel(MainWindow::instance()->rasterOverlayModel());
    rasterOverlays = new QListView;
    rasterOverlays->setModel(rasterOverlayModel);
    rasterOverlays->setModelColumn(RasterOverlayModel::Translated);
    rasterOverlays->setFixedHeight(100);

    /* Moving widget */
    movingWidget = new MovingWidget;

    /* Zoom slider */
    zoomSlider = new ZoomSlider;

    /* Layout */
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Map view:")));
    layout->addWidget(mapView);
    layout->addWidget(new QLabel(tr("Maps:")));
    layout->addWidget(rasterPackages);
    layout->addWidget(new QLabel(tr("Map layer, overlays:")));
    layout->addWidget(rasterLayers);
    layout->addWidget(rasterOverlays);
    layout->addWidget(movingWidget, 0, Qt::AlignCenter);
    layout->addWidget(zoomSlider, 1, Qt::AlignHCenter);
    setLayout(layout);

    setFixedWidth(200);

    /* Update data when raster model is changed */
    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(connectMapView()));
}

void MapOptionsDock::setMapView(int id) {
    if(id == -1) return;

    AbstractMapView* view = MainWindow::instance()->mapViewPluginManager()->instance(mapView->model()->index(id, PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->setMapView(view);
}

void MapOptionsDock::setActualLayer(const QString& layer) {
    rasterLayers->setCurrentIndex(MainWindow::instance()->rasterLayerModel()->find(layer).row());
}

void MapOptionsDock::setActualLayer(int id) {
    AbstractMapView* mapView = MainWindow::instance()->mapView();
    if(!mapView) return;

    mapView->setLayer(MainWindow::instance()->rasterLayerModel()->index(id, RasterLayerModel::Name).data().toString());
}

void MapOptionsDock::connectMapView() {
    AbstractMapView* mapView = MainWindow::instance()->mapView();
    if(!mapView) return;
    connect(mapView, SIGNAL(layerChanged(QString)), SLOT(setActualLayer(QString)));
    connect(mapView, SIGNAL(overlaysChanged(QStringList)), rasterOverlayModel, SLOT(reload(QStringList)));
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
        rasterModelName = QString::fromStdString(*model->metadata()->name());

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
    AbstractMapView* mapView = MainWindow::instance()->mapView();

    if(mapView) {
        QStringList loadedOverlays = mapView->overlays();
        reload(loadedOverlays);

    } else {
        beginResetModel();
        loaded.fill(false, sourceModel()->rowCount());
        endResetModel();
    }
}

void MapOptionsDock::EditableRasterOverlayModel::reload(const QStringList& loadedOverlays) {
    beginResetModel();

    loaded.fill(false, sourceModel()->rowCount());
    for(int row = 0; row != sourceModel()->rowCount(); ++row) {
        if(loadedOverlays.contains(sourceModel()->index(row, 0).data().toString()))
            loaded.setBit(row, true);
    }

    endResetModel();
}

QVariant MapOptionsDock::EditableRasterOverlayModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::CheckStateRole && index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount())
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QAbstractProxyModel::data(index, role);
}

Qt::ItemFlags MapOptionsDock::EditableRasterOverlayModel::flags(const QModelIndex& index) const {
    if(index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount())
        return QAbstractProxyModel::flags(index)|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index);
}

bool MapOptionsDock::EditableRasterOverlayModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    AbstractMapView* mapView = MainWindow::instance()->mapView();

    if(mapView && index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount() && role == Qt::CheckStateRole) {
        /* Remove overlay */
        if((loaded.at(index.row()) && mapView->removeOverlay(data(index.sibling(index.row(), RasterOverlayModel::Name)).toString())) ||
          (!loaded.at(index.row()) && mapView->addOverlay(data(index.sibling(index.row(), RasterOverlayModel::Name)).toString())))
            return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

}}
