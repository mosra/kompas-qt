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

#include "SaveRasterWizard.h"

#include <cmath>
#include <QtCore/QVariant>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>

#include "MainWindow.h"
#include "TileLayerModel.h"
#include "TileOverlayModel.h"
#include "TileZoomModel.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

SaveRasterWizard::SaveRasterWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), areaType(VisibleArea) {
    addPage(new AreaPage(this));
    addPage(new ZoomPage(this));
    addPage(new LayersPage(this));
    addPage(new StatisticsPage(this));
    addPage(new DownloadPage(this));

    /* Modify commit button text */
    setButtonText(CommitButton, tr("Download"));

    setWindowTitle(tr("Save map as..."));
}

SaveRasterWizard::AreaPage::AreaPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("Map area"));
    setSubTitle(tr("Select map area which you want to save."));

    /* Bold font */
    QFont boldFont;
    boldFont.setBold(true);

    /* Map area */
    wholeMap = new QRadioButton(tr("Whole map"));
    wholeMap->setFont(boldFont);
    QLabel* wholeMapLabel = new QLabel(tr("<strong>Be careful</strong>, if you "
        "are trying to save map of whole world, select only a few small zoom "
        "levels, otherwise the download could take forever, it won't fit onto "
        "your hard drive and your IP will be banned from using that map server "
        "ever again."));
    wholeMapLabel->setWordWrap(true);
    wholeMapLabel->setAlignment(Qt::AlignJustify);
    visibleArea = new QRadioButton(tr("Visible area"));
    visibleArea->setFont(boldFont);
    visibleArea->setChecked(true);
    QLabel* visibleAreaLabel = new QLabel(tr("There could be some margin "
        "outside the visible area because map is divided into tiles -  whole "
        "tile will be downloaded even if only small portion of it is visible."));
    visibleAreaLabel->setWordWrap(true);
    visibleAreaLabel->setAlignment(Qt::AlignJustify);
    customArea = new QRadioButton(tr("Custom area"));
    customArea->setFont(boldFont);
    customArea->setDisabled(true);
    customAreaSelect = new QPushButton(tr("Select..."));
    customAreaSelect->setDisabled(true);
    connect(customArea, SIGNAL(toggled(bool)), customAreaSelect, SLOT(setEnabled(bool)));
    QLabel* customAreaLabel = new QLabel(tr("This is good if you want to have "
        "the smallest package size without unneccessary margins."));
    customAreaLabel->setWordWrap(true);
    customAreaLabel->setAlignment(Qt::AlignJustify);
    customAreaLabel->setDisabled(true);

    QHBoxLayout* customAreaLayout = new QHBoxLayout;
    customAreaLayout->addWidget(customArea);
    customAreaLayout->addWidget(customAreaSelect, 1, Qt::AlignLeft);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(wholeMap, 0, 0, 1, 2);
    layout->addWidget(wholeMapLabel, 1, 0, 1, 2);
    layout->addWidget(visibleArea, 2, 0, 1, 2);
    layout->addWidget(visibleAreaLabel, 3, 0, 1, 2);
    layout->addLayout(customAreaLayout, 4, 0, 1, 2);
    layout->addWidget(customAreaLabel, 5, 0, 1, 2);

    setLayout(layout);
}

bool SaveRasterWizard::AreaPage::validatePage() {
    if(wholeMap->isChecked())
        wizard->areaType = SaveRasterWizard::WholeMap;
    else if(customArea->isChecked())
        wizard->areaType = SaveRasterWizard::CustomArea;
    else
        wizard->areaType = SaveRasterWizard::VisibleArea;

    return true;
}

SaveRasterWizard::ZoomPage::ZoomPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("Zoom levels"));
    setSubTitle(tr("Select zoom levels which you want to save."));

    /* Zoom levels */
    minZoom = new QSpinBox;
    maxZoom = new QSpinBox;
    QLabel* zoomLabel = new QLabel(tr("Each more detailed zoom level will "
        "increase package size (and download time) five times."));
    zoomLabel->setWordWrap(true);
    zoomLabel->setAlignment(Qt::AlignJustify);

    /* Limits for zoom, current zoom */
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    int minimum = model->zoomLevels().front();
    int maximum = model->zoomLevels().back();
    MainWindow::instance()->unlockRasterModel();
    int current = (*MainWindow::instance()->mapView())->zoom();

    /* Set maximum, minimum and current zoom value */
    minZoom->setMinimum(minimum);
    maxZoom->setMinimum(minimum);
    minZoom->setMaximum(maximum);
    maxZoom->setMaximum(maximum);
    minZoom->setValue(current);
    maxZoom->setValue(current);

    /* Zoom range checks */
    connect(minZoom, SIGNAL(valueChanged(int)), SLOT(checkMinValue(int)));
    connect(maxZoom, SIGNAL(valueChanged(int)), SLOT(checkMaxValue(int)));

    QGridLayout* basicLayout = new QGridLayout;
    basicLayout->addWidget(new QLabel(tr("Minimal zoom:")), 0, 0);
    basicLayout->addWidget(minZoom, 0, 1);
    basicLayout->addWidget(new QLabel(tr("Maximal zoom:")), 1, 0);
    basicLayout->addWidget(maxZoom, 1, 1);
    basicLayout->addWidget(zoomLabel, 2, 0, 1, 2);
    basic = new QGroupBox;
    basic->setFlat(true);
    basic->setLayout(basicLayout);

    /* List view for all zoom levels */
    zoomLevelsView = new QListView;
    zoomLevelsView->setModel(new TileZoomModel(this));
    zoomLevelsView->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(zoomLevelsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SIGNAL(completeChanged()));

    QHBoxLayout* advancedLayout = new QHBoxLayout;
    advancedLayout->addWidget(zoomLevelsView);
    advanced = new QGroupBox;
    advanced->setFlat(true);
    advanced->setDisabled(true);
    advanced->setLayout(advancedLayout);

    basicButton = new QRadioButton(tr("Basic: select zoom range"));
    basicButton->setChecked(true);
    advancedButton = new QRadioButton(tr("Advanced: select zoom levels"));
    advancedButton->setChecked(false);
    connect(basicButton, SIGNAL(toggled(bool)), SLOT(switchGroups()));
    connect(advancedButton, SIGNAL(toggled(bool)), SLOT(switchGroups()));

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(basicButton, 0, 0);
    layout->addWidget(advancedButton, 0, 1);
    layout->addWidget(basic, 1, 0);
    layout->addWidget(advanced, 1, 1);

    setLayout(layout);
}

void SaveRasterWizard::ZoomPage::switchGroups() {
    /* Enable basic (zoom range) */
    if(basicButton->isChecked()) {
        basic->setEnabled(true);
        advanced->setDisabled(true);

        /* Set zoom range to first and last selected zoom level in advanced */
        QModelIndexList selection = zoomLevelsView->selectionModel()->selectedRows();

        /* Don't change anything if nothing selected or if user didn't change
           anything in advanced section */
        if(selection.isEmpty()) return;

        /* Get first and last selected index */
        int min = maxZoom->value();
        int max = minZoom->value();
        foreach(const QModelIndex& index, selection) {
            int value = index.data().toInt();
            if(value < min) min = value;
            if(value > max) max = value;
        }

        minZoom->setValue(min);
        maxZoom->setValue(max);

    /* Enable advanced (zoom levels) */
    } else {
        basic->setDisabled(true);
        advanced->setEnabled(true);

        QItemSelectionModel* selection = zoomLevelsView->selectionModel();

        selection->select(QItemSelection(
            zoomLevelsView->model()->index(minZoom->value()-minZoom->minimum(), 0),
            zoomLevelsView->model()->index(maxZoom->value()-maxZoom->minimum(), 0)
        ), QItemSelectionModel::ClearAndSelect);
    }
}

bool SaveRasterWizard::ZoomPage::isComplete() const {
    if(advancedButton->isChecked() && zoomLevelsView->selectionModel()->selectedIndexes().isEmpty())
        return false;

    return true;
}

bool SaveRasterWizard::ZoomPage::validatePage() {
    wizard->zoomLevels.clear();

    /* Save zoom range */
    if(basicButton->isChecked()) {
        for(int i = minZoom->value(); i <= maxZoom->value(); ++i)
            wizard->zoomLevels << i;

    /* Save each selected level */
    } else {
        QModelIndexList list = zoomLevelsView->selectionModel()->selectedIndexes();
        if(list.isEmpty()) return false;

        foreach(const QModelIndex& index, list)
            wizard->zoomLevels << index.data().toUInt();

        /* Sort the list ascending */
        qSort(wizard->zoomLevels);
    }

    return true;
}

SaveRasterWizard::LayersPage::LayersPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("Map layers"));
    setSubTitle(tr("Select tiles and overlays to save."));

    layersView = new QListView;
    layersView->setSelectionMode(QAbstractItemView::MultiSelection);
    layersView->setModel(new TileLayerModel);
    connect(layersView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SIGNAL(completeChanged()));

    overlaysView = new QListView;
    overlaysView->setSelectionMode(QAbstractItemView::MultiSelection);
    overlaysView->setModel(new TileOverlayModel);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Layers:")), 0, 0);
    layout->addWidget(new QLabel(tr("Overlays:")), 0, 1);
    layout->addWidget(layersView, 1, 0);
    layout->addWidget(overlaysView, 1, 1);

    setLayout(layout);
}

bool SaveRasterWizard::LayersPage::isComplete() const {
    if(layersView->selectionModel()->selectedIndexes().isEmpty())
        return false;

    return true;
}

bool SaveRasterWizard::LayersPage::validatePage() {
    wizard->layers.clear();
    wizard->overlays.clear();

    /* Save layers */
    QModelIndexList layerList = layersView->selectionModel()->selectedIndexes();
    if(layerList.isEmpty()) return false;
    foreach(const QModelIndex& index, layerList)
        wizard->layers << index.data().toString();

    /* Save overlays */
    QModelIndexList overlayList = overlaysView->selectionModel()->selectedIndexes();
    foreach(const QModelIndex& index, overlayList)
        wizard->overlays << index.data().toString();

    return true;
}

SaveRasterWizard::StatisticsPage::StatisticsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), canDownload(true) {
    setTitle(tr("Statistics"));
    setSubTitle(tr("Review amount of data to be downloaded, return back and make changes or proceed to download."));
    setCommitPage(true);

    QFont boldFont;
    boldFont.setBold(true);

    tileCountMinZoom = new QLabel;
    zoomLevelCount = new QLabel;
    tileCountOneLayer = new QLabel;
    layerCount = new QLabel;
    tileCountTotal = new QLabel;
    downloadSize = new QLabel;
    downloadSize->setFont(boldFont);
    fupWarning = new QLabel;
    fupWarning->setWordWrap(true);
    fupWarning->setFont(boldFont);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Tile count in lowest zoom level:")), 0, 0);
    layout->addWidget(tileCountMinZoom, 0, 1);
    layout->addWidget(new QLabel(tr("Number of zoom levels:")), 1, 0);
    layout->addWidget(zoomLevelCount, 1, 1);
    layout->addWidget(new QLabel(tr("Tile count for one layer:")), 2, 0);
    layout->addWidget(tileCountOneLayer, 2, 1);
    layout->addWidget(new QLabel(tr("Number of layers:")), 3, 0);
    layout->addWidget(layerCount, 3, 1);
    layout->addWidget(new QLabel(tr("Total tile count:")), 4, 0);
    layout->addWidget(tileCountTotal, 4, 1);
    layout->addWidget(new QLabel(tr("Estimated download size (10 kB for one tile):")), 5, 0);
    layout->addWidget(downloadSize, 5, 1);
    layout->addWidget(fupWarning, 6, 0, 1, 2);

    setLayout(layout);
}

void SaveRasterWizard::StatisticsPage::initializePage() {
    AbstractMapView* mapView = *MainWindow::instance()->mapView();

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    Zoom minAvailableZoom = rasterModel->zoomLevels()[0];
    double zoomMultiplier = rasterModel->zoomStep()*rasterModel->zoomStep();
    MainWindow::instance()->unlockRasterModel();

    quint64 _tileCountMinZoom;

    /* Tile count for whole map at _lowest possible_ zoom */
    if(wizard->areaType == SaveRasterWizard::WholeMap) {
        TileArea area = rasterModel->area();
        _tileCountMinZoom = area.w*area.h*pow(zoomMultiplier, wizard->zoomLevels[0]-minAvailableZoom);

    /* Tile count for visible area at _current_ zoom */
    } else if(wizard->areaType == SaveRasterWizard::VisibleArea) {
        /* Tile coordinates in visible area, divide them for smallest zoom */
        AbsoluteArea<unsigned int> tiles = mapView->tilesInArea();
        tiles = tiles/pow(zoomMultiplier, mapView->zoom()-wizard->zoomLevels[0]);

        _tileCountMinZoom = (tiles.x2-tiles.x1+1)*(tiles.y2-tiles.y1+1);

    /* Tile count for selected area at _current_ zoom */
    } else {
        /** @todo CustomArea to be implemented... */
    }

    tileCountMinZoom->setText(QString::number(_tileCountMinZoom));
        zoomLevelCount->setText(QString::number(wizard->zoomLevels.size()));

    /* Tile count for all zoom levels */
    quint64 _tileCountOneLayer = 0;
    foreach(Zoom z, wizard->zoomLevels)
        _tileCountOneLayer += _tileCountMinZoom*pow(zoomMultiplier, z-wizard->zoomLevels[0]);

    tileCountOneLayer->setText(QString::number(_tileCountOneLayer));

    unsigned int _layerCount = wizard->layers.size()+wizard->overlays.size();
    layerCount->setText(QString::number(_layerCount));

    quint64 _tileCountTotal = _tileCountOneLayer*_layerCount;
    tileCountTotal->setText(QString::number(_tileCountTotal));

    /* Download size okay, don't display anything */
    if(_tileCountTotal < 104857) {
        fupWarning->setText("");
        canDownload = true;

    /* Donwload size over 1 GB, display warning */
    } else if(_tileCountTotal < 1048576) {
        fupWarning->setText("Download size is over 1 GB. Consider selecting "
            "smaller data amount, because this download can lead to pernament "
            "IP ban.");
        canDownload = true;

    /* Download size over 10 GB, don't allow download */
    } else {
        fupWarning->setText("Download size is over 10 GB. Please select smaller "
            "data amount or the download will not be allowed.");
        canDownload = false;
    }

    emit completeChanged();

    /* Download size (10 kB for one tile) */
    downloadSize->setText(QString("%0 MB").arg(_tileCountTotal/102.4, 0, 'f', 1));
}

SaveRasterWizard::DownloadPage::DownloadPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("Downloading..."));
    setSubTitle(tr("The data are now being downloaded and saved to your package."));
}

}}
