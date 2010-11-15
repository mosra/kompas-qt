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

MapOptionsDock::MapOptionsDock(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    QFont font;
    font.setBold(true);

    rasterModelName = new QLabel;
    rasterModelName->setFont(font);

    rasterModelOnline = new QCheckBox(tr("Enable online maps"));
    rasterModelOnline->setChecked(MainWindow::instance()->configuration()->group("map")->value<bool>("online"));
    connect(rasterModelOnline, SIGNAL(clicked(bool)), MainWindow::instance(), SLOT(setOnlineEnabled(bool)));

    /* Raster layers combobox */
    rasterLayers = new QComboBox;
    rasterLayers->setModel(MainWindow::instance()->rasterLayerModel());

    /* Raster overlays list view */
    EditableRasterOverlayModel* editableRasterOverlayModel = new EditableRasterOverlayModel(MainWindow::instance()->mapView(), this);
    editableRasterOverlayModel->setSourceModel(MainWindow::instance()->rasterOverlayModel());
    rasterOverlays = new QListView;
    rasterOverlays->setModel(editableRasterOverlayModel);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(rasterModelName, 0, 0, 1, 2);
    layout->addWidget(rasterModelOnline, 1, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Map layer:")), 2, 0);
    layout->addWidget(rasterLayers, 2, 1);
    layout->addWidget(new QLabel(tr("Overlays:")), 3, 0, 1, 2);
    layout->addWidget(rasterOverlays, 4, 0, 1, 2);
    layout->addWidget(new QWidget, 5, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(5, 1);
    setLayout(layout);

    setActualData();

    /* Update data when raster model is changed */
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged()), SLOT(setActualData()));

    /* Connect comboboxes with model / layer changing */
    /** @todo Make it non-dependent on one map view */
    connect(rasterLayers, SIGNAL(currentIndexChanged(QString)), *MainWindow::instance()->mapView(), SLOT(setLayer(QString)));
}

void MapOptionsDock::setActualData() {
    /* Display actual model name */
    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    /* Raster model is loaded, enable and display its name */
    if(rasterModel) {
        setDisabled(false);

        rasterModelName->setText(QString::fromStdString(MainWindow::instance()->rasterModelPluginManager()->metadata(rasterModel->name())->name()));

        /* Enable online maps enablenator if they are supported */
        if(rasterModel->features() & AbstractRasterModel::LoadableFromUrl) {
            rasterModelOnline->setDisabled(false);

            if(rasterModel->online())
                rasterModelOnline->setChecked(true);
            else
                rasterModelOnline->setChecked(false);

        } else {
            rasterModelOnline->setDisabled(true);
            rasterModelOnline->setChecked(false);
        }

    /* No raster model loaded, disable widget */
    } else {
        setDisabled(true);

        rasterModelName->setText(tr("No map loaded"));
        rasterModelOnline->setChecked(false);
    }

    /* Set actual map layer */
    rasterLayers->setCurrentIndex(rasterLayers->findText((*MainWindow::instance()->mapView())->layer()));

    /** @todo Actual overlays? */

    MainWindow::instance()->unlockRasterModel();
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
