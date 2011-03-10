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

#include "ContentsPage.h"

#include <algorithm>
#include <QtGui/QListView>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "SaveRasterWizard.h"
#include "MainWindow.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "RasterZoomModel.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

ContentsPage::ContentsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("2/5: Map contents"));
    setSubTitle(tr("Select zoom levels, layers and overlays to save."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/progress2-48.png"));

    MainWindow* m = MainWindow::instance();
    AbstractMapView* view = m->mapView();

    zoomLevelsView = new QListView;
    zoomLevelsView->setSelectionMode(QAbstractItemView::MultiSelection);
    zoomLevelsView->setModel(m->rasterZoomModel());
    connect(zoomLevelsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SIGNAL(completeChanged()));

    layersView = new QListView;
    layersView->setSelectionMode(QAbstractItemView::MultiSelection);
    layersView->setModel(m->rasterLayerModel());
    layersView->setModelColumn(RasterLayerModel::Translated);
    connect(layersView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SIGNAL(completeChanged()));

    overlaysView = new QListView;
    overlaysView->setSelectionMode(QAbstractItemView::MultiSelection);
    overlaysView->setModel(m->rasterOverlayModel());
    overlaysView->setModelColumn(RasterOverlayModel::Translated);

    /* Select current zoom and layers for convenience */
    zoomLevelsView->selectionModel()->select(m->rasterZoomModel()->find(view->zoom()), QItemSelectionModel::Select);

    QModelIndex current = m->rasterLayerModel()->find(view->layer());
    layersView->selectionModel()->select(current.sibling(current.row(), RasterLayerModel::Translated), QItemSelectionModel::Select);

    foreach(const QString& overlay, view->overlays()) {
        current = m->rasterOverlayModel()->find(overlay);
        overlaysView->selectionModel()->select(current.sibling(current.row(), RasterLayerModel::Translated), QItemSelectionModel::Select);
    }

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Zoom levels:")), 0, 0);
    layout->addWidget(new QLabel(tr("Layers:")), 0, 1);
    layout->addWidget(new QLabel(tr("Overlays:")), 0, 2);
    layout->addWidget(zoomLevelsView, 1, 0);
    layout->addWidget(layersView, 1, 1);
    layout->addWidget(overlaysView, 1, 2);

    setLayout(layout);
}

bool ContentsPage::isComplete() const {
    if(zoomLevelsView->selectionModel()->selectedIndexes().isEmpty() || layersView->selectionModel()->selectedIndexes().isEmpty())
        return false;

    return true;
}

bool ContentsPage::validatePage() {
    wizard->zoomLevels.clear();
    wizard->layers.clear();
    wizard->overlays.clear();

    /* Zoom levels */
    QModelIndexList zoomLevelsList = zoomLevelsView->selectionModel()->selectedIndexes();
    if(zoomLevelsList.isEmpty()) return false;
    foreach(const QModelIndex& index, zoomLevelsList)
        wizard->zoomLevels.push_back(index.data().toUInt());

    /* Sort zoom levels ascending */
    sort(wizard->zoomLevels.begin(), wizard->zoomLevels.end());

    /* Save layers */
    QModelIndexList layerList = layersView->selectionModel()->selectedIndexes();
    if(layerList.isEmpty()) return false;
    foreach(const QModelIndex& index, layerList)
        wizard->layers.push_back(index.sibling(index.row(), RasterLayerModel::Name).data().toString().toStdString());

    /* Save overlays */
    QModelIndexList overlayList = overlaysView->selectionModel()->selectedIndexes();
    foreach(const QModelIndex& index, overlayList)
        wizard->overlays.push_back(index.sibling(index.row(), RasterOverlayModel::Name).data().toString().toStdString());

    return true;
}

}}}
