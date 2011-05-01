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

#include "DownloadPage.h"

#include <QtCore/QTimer>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

#include "SaveRasterWizard.h"
#include "SaveRasterThread.h"
#include "MessageBox.h"
#include "MainWindow.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

DownloadPage::DownloadPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), _isComplete(false) {
    setTitle(tr("5/5: Downloading..."));
    setSubTitle(tr("The data are now being downloaded and saved to your package."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/progress5-48.png"));

    saveThread = new SaveRasterThread(this);
    connect(saveThread, SIGNAL(completeChanged(Core::Zoom,int,std::string,int,int,int)), SLOT(updateStatus(Core::Zoom,int,std::string,int,int,int)));
    connect(saveThread, SIGNAL(error()), SLOT(error()));
    connect(saveThread, SIGNAL(completed()), SLOT(completed()));

    filename = new QLabel;
    currentZoom = new QLabel;
    currentLayer = new QLabel;
    totalCompleted = new QProgressBar;
    currentZoomLayerCompleted = new QProgressBar;

    QCheckBox* openWhenFinished = new QCheckBox(tr("Open the package when finished"));
    openWhenFinished->setChecked(wizard->openWhenFinished);
    connect(openWhenFinished, SIGNAL(toggled(bool)), SLOT(setOpenWhenFinished(bool)));

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(filename, 0, 0, 1, 2);
    layout->addWidget(currentZoom, 1, 0);
    layout->addWidget(currentLayer, 1, 1);
    layout->addWidget(new QLabel(tr("Current layer/zoom progress:")), 2, 0, 1, 2);
    layout->addWidget(currentZoomLayerCompleted, 3, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Total progress:")), 4, 0, 1, 2);
    layout->addWidget(totalCompleted, 5, 0, 1, 2);
    layout->addWidget(openWhenFinished, 6, 0, 1, 2);
    layout->setRowMinimumHeight(1, 48);
    layout->setRowMinimumHeight(6, 48);
    setLayout(layout);
}

void DownloadPage::initializePage() {
    filename->setText(tr("Initializing package %0 ...").arg(QString::fromStdString(wizard->filename)));

    /* Compute area at lowest zoom level */
    TileArea area = wizard->area();

    updateStatus(0, 0, "", 0, 0, 0);

    if(!saveThread->initializePackage(wizard->model, wizard->filename, wizard->tileSize, wizard->zoomLevels, area, wizard->layers, wizard->overlays)) {
        filename->setText(tr("Failed to initialize package %0").arg(QString::fromStdString(wizard->filename)));
        QTimer::singleShot(0, this, SIGNAL(error()));
        return;
    }

    if(!wizard->name.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Name, wizard->name);
    if(!wizard->description.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Description, wizard->description);
    if(!wizard->packager.empty())
        saveThread->setPackageAttribute(AbstractRasterModel::Packager, wizard->packager);

    saveThread->start();
}

void DownloadPage::updateStatus(Zoom _currentZoom, int currentZoomNumber, const string& _currentLayer, int currentLayerNumber, int _totalCompleted, int _currentZoomLayerCompleted) {
    filename->setText(tr("Downloading and saving data ..."));

    /* Get translated layer name */
    QString layer;
    QModelIndex found = MainWindow::instance()->rasterLayerModel()->find(QString::fromStdString(_currentLayer));
    if(found.isValid()) layer = found.sibling(found.row(), RasterLayerModel::Translated).data().toString();
    else {
        found = MainWindow::instance()->rasterOverlayModel()->find(QString::fromStdString(_currentLayer));
        if(found.isValid()) layer = found.sibling(found.row(), RasterOverlayModel::Translated).data().toString();
        else layer = QString::fromStdString(_currentLayer);
    }

    currentZoom->setText(tr("Current zoom: %0 (%1/%2)").arg(_currentZoom).arg(currentZoomNumber).arg(wizard->zoomLevels.size()));
    currentLayer->setText(tr("Current layer: %0 (%1/%2)").arg(layer).arg(currentLayerNumber).arg(wizard->layers.size()+wizard->overlays.size()));
    totalCompleted->setValue(_totalCompleted);
    currentZoomLayerCompleted->setValue(_currentZoomLayerCompleted);
}

void DownloadPage::completed() {
    filename->setText(tr("Package completed."));

    _isComplete = true;
    wizard->button(QWizard::CancelButton)->setDisabled(true);
    emit completeChanged();

    MessageBox::information(this, tr("Package completed"), tr("Package is successfully completed."));
}

void DownloadPage::error() {
    filename->setText(tr("Error while creating package."));

    _isComplete = true;
    wizard->button(QWizard::CancelButton)->setDisabled(true);
    emit completeChanged();

    MessageBox::critical(this, tr("Packaging error"), tr("Something bad happened during package creation."));
}

void DownloadPage::setOpenWhenFinished(bool open) {
    wizard->openWhenFinished = open;
}

}}}
