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
#include <algorithm>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QProgressBar>

#include "MainWindow.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "RasterZoomModel.h"
#include "SaveRasterThread.h"
#include "MessageBox.h"

using namespace std;
using namespace Map2X::Utility;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

SaveRasterWizard::SaveRasterWizard(const string& _model, QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), model(_model) {
    addPage(new ZoomPage(this));
    addPage(new AreaPage(this));
    addPage(new LayersPage(this));
    addPage(new MetadataPage(this));
    addPage(new StatisticsPage(this));
    addPage(new DownloadPage(this));

    setOptions(NoBackButtonOnStartPage|NoBackButtonOnLastPage);
    setButtonText(CommitButton, tr("Download"));
    setWindowTitle(tr("Save map as..."));

    /* Save tile size */
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    tileSize = model->tileSize();
    MainWindow::instance()->unlockRasterModel();
}

TileArea SaveRasterWizard::area() const {
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    TileArea area = model->area()*pow2(zoomLevels[0]-model->zoomLevels()[0]);
    MainWindow::instance()->unlockRasterModel();

    /* Tile area at minimal zoom */
    TileArea ta(
        area.x+absoluteArea.x1*area.w,
        area.y+absoluteArea.y1*area.h,
        ceil((absoluteArea.x2-absoluteArea.x1)*area.w),
        ceil((absoluteArea.y2-absoluteArea.y1)*area.h)
    );

    return ta;
}

SaveRasterWizard::AreaPage::AreaPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("2/6: Map area"));
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
    /* Tile count for whole map at _lowest possible_ zoom */
    if(wholeMap->isChecked())
        wizard->absoluteArea = AbsoluteArea<double>(0, 0, 1, 1);

    /* Tile count for visible area at _current_ zoom */
    else if(visibleArea->isChecked()) {
        AbstractMapView* mapView = MainWindow::instance()->mapView();

        /* Tile coordinates in visible area, divide them for smallest zoom */
        wizard->absoluteArea = mapView->viewedArea();

    /* Tile count for selected area at _current_ zoom */
    } else {
        /** @todo CustomArea to be implemented... */
    }

    return true;
}

SaveRasterWizard::ZoomPage::ZoomPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("1/6: Zoom levels"));
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
    int current = MainWindow::instance()->mapView()->zoom();

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
    zoomLevelsView->setModel(MainWindow::instance()->rasterZoomModel());
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
            wizard->zoomLevels.push_back(i);

    /* Save each selected level */
    } else {
        QModelIndexList list = zoomLevelsView->selectionModel()->selectedIndexes();
        if(list.isEmpty()) return false;

        foreach(const QModelIndex& index, list)
            wizard->zoomLevels.push_back(index.data().toUInt());

        /* Sort the list ascending */
        sort(wizard->zoomLevels.begin(), wizard->zoomLevels.end());
    }

    return true;
}

SaveRasterWizard::LayersPage::LayersPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("3/6: Map layers"));
    setSubTitle(tr("Select tiles and overlays to save."));

    layersView = new QListView;
    layersView->setSelectionMode(QAbstractItemView::MultiSelection);
    layersView->setModel(MainWindow::instance()->rasterLayerModel());
    connect(layersView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SIGNAL(completeChanged()));

    overlaysView = new QListView;
    overlaysView->setSelectionMode(QAbstractItemView::MultiSelection);
    overlaysView->setModel(MainWindow::instance()->rasterOverlayModel());

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
        wizard->layers.push_back(index.data().toString().toStdString());

    /* Save overlays */
    QModelIndexList overlayList = overlaysView->selectionModel()->selectedIndexes();
    foreach(const QModelIndex& index, overlayList)
        wizard->overlays.push_back(index.data().toString().toStdString());

    return true;
}

SaveRasterWizard::StatisticsPage::StatisticsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), canDownload(true) {
    setTitle(tr("5/6: Statistics"));
    setSubTitle(tr("Review amount of data to be downloaded, return back and make changes or proceed to creating the package."));
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
    /* Compute area at lowest zoom level */
    TileArea area = wizard->area();

    tileCountMinZoom->setText(QString::number(area.w*area.h));
        zoomLevelCount->setText(QString::number(wizard->zoomLevels.size()));

    /* Tile count for all zoom levels */
    quint64 _tileCountOneLayer = 0;
    for(vector<Zoom>::const_iterator it = wizard->zoomLevels.begin(); it != wizard->zoomLevels.end(); ++it) {
        TileArea a = area*pow2(*it-wizard->zoomLevels[0]);
        _tileCountOneLayer += static_cast<quint64>(a.w)*a.h;
    }

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

SaveRasterWizard::MetadataPage::MetadataPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("4/6: Metadata"));
    setSubTitle(tr("Select where to save the package and optionally fill in some metadata."));

    /* Initialize widgets */
    filename = new QLineEdit;
    connect(filename, SIGNAL(textChanged(QString)), SIGNAL(completeChanged()));
    name = new QLineEdit;
    description = new QLineEdit;
    packager = new QLineEdit;
    QPushButton* fileButton = new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Select..."));
    connect(fileButton, SIGNAL(clicked(bool)), SLOT(saveFileDialog()));

    QHBoxLayout* fileLayout = new QHBoxLayout;
    fileLayout->addWidget(filename);
    fileLayout->addWidget(fileButton);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Save to:")), 0, 0);
    layout->addLayout(fileLayout, 0, 1);
    layout->addWidget(new QLabel(tr("Package name:")), 1, 0);
    layout->addWidget(name, 1, 1);
    layout->addWidget(new QLabel(tr("Description:")), 2, 0);
    layout->addWidget(description, 2, 1);
    layout->addWidget(new QLabel(tr("Packager:")), 3, 0);
    layout->addWidget(packager, 3, 1);

    setLayout(layout);
}

bool SaveRasterWizard::MetadataPage::isComplete() const {
    if(filename->text().isEmpty()) return false;
    else return true;
}

bool SaveRasterWizard::MetadataPage::validatePage() {
    if(filename->text().isEmpty()) return false;

    wizard->filename = filename->text().toStdString();
    wizard->name = name->text().toStdString();
    wizard->description = description->text().toStdString();
    wizard->packager = packager->text().toStdString();

    return true;
}

void SaveRasterWizard::MetadataPage::saveFileDialog() {
    filename->setText(QFileDialog::getSaveFileName(this, tr("Save package as..."), QString::fromStdString(MainWindow::instance()->configuration()->group("paths")->value<string>("packages"))));
}

SaveRasterWizard::DownloadPage::DownloadPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), _isComplete(false) {
    setTitle(tr("6/6: Downloading..."));
    setSubTitle(tr("The data are now being downloaded and saved to your package."));

    saveThread = new SaveRasterThread(this);
    connect(saveThread, SIGNAL(completeChanged(Core::Zoom,int,std::string,int,int,int)), SLOT(updateStatus(Core::Zoom,int,std::string,int,int,int)));
    connect(saveThread, SIGNAL(error()), SLOT(error()));
    connect(saveThread, SIGNAL(completed()), SLOT(completed()));

    filename = new QLabel;
    currentZoom = new QLabel;
    currentLayer = new QLabel;
    totalCompleted = new QProgressBar;
    currentZoomLayerCompleted = new QProgressBar;

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(filename, 0, 0, 1, 2);
    layout->addWidget(currentZoom, 1, 0);
    layout->addWidget(currentLayer, 1, 1);
    layout->addWidget(new QLabel(tr("Total progress:")), 2, 0, 1, 2);
    layout->addWidget(totalCompleted, 3, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Current layer/zoom progress:")), 4, 0, 1, 2);
    layout->addWidget(currentZoomLayerCompleted, 5, 0, 1, 2);
    layout->setRowMinimumHeight(1, 48);
    setLayout(layout);
}

void SaveRasterWizard::DownloadPage::initializePage() {
    filename->setText(tr("Initializing package %0 ...").arg(QString::fromStdString(wizard->filename)));

    /* Compute area at lowest zoom level */
    TileArea area = wizard->area();

    updateStatus(0, 0, "", 0, 0, 0);

    if(!saveThread->initializePackage(wizard->model, wizard->filename, wizard->tileSize, wizard->zoomLevels, area, wizard->layers, wizard->overlays))
        filename->setText(tr("Failed to initialize package %0").arg(QString::fromStdString(wizard->filename)));

    if(!wizard->name.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Name, wizard->name);
    if(!wizard->description.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Description, wizard->description);
    if(!wizard->packager.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Packager, wizard->packager);

    saveThread->start();
}

void SaveRasterWizard::DownloadPage::updateStatus(Zoom _currentZoom, int currentZoomNumber, const string& _currentLayer, int currentLayerNumber, int _totalCompleted, int _currentZoomLayerCompleted) {
    filename->setText(tr("Downloading and saving data ..."));

    currentZoom->setText(tr("Current zoom: %0 (%1/%2)").arg(_currentZoom).arg(currentZoomNumber).arg(wizard->zoomLevels.size()));
    currentLayer->setText(tr("Current layer: %0 (%1/%2)").arg(QString::fromStdString(_currentLayer)).arg(currentLayerNumber).arg(wizard->layers.size()+wizard->overlays.size()));
    totalCompleted->setValue(_totalCompleted);
    currentZoomLayerCompleted->setValue(_currentZoomLayerCompleted);
}

void SaveRasterWizard::DownloadPage::completed() {
    filename->setText(tr("Package completed."));

    _isComplete = true;
    wizard->button(CancelButton)->setDisabled(true);
    emit completeChanged();

    MessageBox::information(this, tr("Package completed"), tr("Package is successfully completed."));
}

void SaveRasterWizard::DownloadPage::error() {
    filename->setText(tr("Error while creating package."));

    _isComplete = true;
    wizard->button(CancelButton)->setDisabled(true);
    emit completeChanged();

    MessageBox::critical(this, tr("Packaging error"), tr("Something bad happened during package creation."));
}

}}
