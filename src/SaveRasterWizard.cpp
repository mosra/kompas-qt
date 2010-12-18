/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
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
#include "PluginManager.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

SaveRasterWizard::SaveRasterWizard(const string& _model, QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), model(_model) {
    addPage(new AreaPage(this));
    addPage(new ContentsPage(this));
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

int SaveRasterWizard::exec() {
    /* Tile size and features of source model. Set ConvertableCoords feature
        only if the map has valid projection */
    const AbstractRasterModel* sourceModel = MainWindow::instance()->lockRasterModelForRead();
    int sourceFeatures = sourceModel->features() & (sourceModel->projection() ?
        AbstractRasterModel::ConvertableCoords :
        ~AbstractRasterModel::ConvertableCoords);
    MainWindow::instance()->unlockRasterModel();

    /* Features of destination model */
    AbstractRasterModel* destinationModel = MainWindow::instance()->rasterModelPluginManager()->instance(model);
    int destinationFeatures;
    if(!destinationModel) destinationFeatures = sourceFeatures; /* will fail in download page too, so don't bother */
    else destinationFeatures = destinationModel->features();
    delete destinationModel;

    /* Check what features are missing in destination model */
    QString log;
    for(int i = 0; i != 32; ++i) {
        int feature = 1 << i;

        /* If feature doesn't exist in source model or is present in destination
            model, go to next feature */
        if(!(feature & sourceFeatures) || (feature & destinationFeatures))
            continue;

        switch(feature) {
            case AbstractRasterModel::ConvertableCoords:
                log += QString("<li><strong>%0</strong> - %1</li>").arg(tr("No GPS coordinates support")).arg(tr("you won't be able to measure distances.")); break;
        }
    }

    if(!log.isEmpty()) {
        if(MessageBox::question(this, tr("Data loss warning"), tr("You are saving to a format which doesn't support all features provided by the map:<ul>%0</ul>Do you want to continue?").arg(log), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) return QWizard::Rejected;
    }

    return QWizard::exec();
}

TileArea SaveRasterWizard::area() const {
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    TileArea area = model->area()*pow2(zoomLevels[0]-*model->zoomLevels().begin());
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
    setTitle(tr("1/5: Map area"));
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

SaveRasterWizard::ContentsPage::ContentsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("2/5: Map contents"));
    setSubTitle(tr("Select zoom levels, layers and overlays to save."));

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

bool SaveRasterWizard::ContentsPage::isComplete() const {
    if(zoomLevelsView->selectionModel()->selectedIndexes().isEmpty() || layersView->selectionModel()->selectedIndexes().isEmpty())
        return false;

    return true;
}

bool SaveRasterWizard::ContentsPage::validatePage() {
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

SaveRasterWizard::StatisticsPage::StatisticsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), canDownload(true) {
    setTitle(tr("4/5: Statistics"));
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
    setTitle(tr("3/5: Metadata"));
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
    setTitle(tr("5/5: Downloading..."));
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
