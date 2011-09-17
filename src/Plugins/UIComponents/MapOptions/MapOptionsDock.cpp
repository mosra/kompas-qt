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

#include "EditableRasterPackageModel.h"
#include "EditableRasterOverlayModel.h"
#include "RasterPackageModel.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "PluginManager.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "PluginManagerStore.h"
#include "MovingWidget.h"
#include "ZoomSlider.h"

using namespace std;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MapOptionsDock::MapOptionsDock(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    PluginModel* mapViewModel = MainWindow::instance()->pluginManagerStore()->mapViews()->loadedOnlyModel();
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
    rasterPackages->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
    rasterOverlays->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    AbstractMapView* view = MainWindow::instance()->pluginManagerStore()->mapViews()->manager()->instance(mapView->model()->index(id, PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->setMapView(view);
}

void MapOptionsDock::setActualLayer(const QString& layer) {
    rasterLayers->setCurrentIndex(MainWindow::instance()->rasterLayerModel()->find(layer).row());
}

void MapOptionsDock::setActualLayer(int id) {
    if(id == -1) return;
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

}}}
