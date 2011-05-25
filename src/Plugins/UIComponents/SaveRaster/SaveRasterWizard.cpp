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
#include <QtGui/QCheckBox>

#include "MainWindow.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "RasterZoomModel.h"
#include "SaveRasterThread.h"
#include "MessageBox.h"
#include "PluginManager.h"
#include "AreaPage.h"
#include "ContentsPage.h"
#include "DownloadPage.h"
#include "MetadataPage.h"
#include "StatisticsPage.h"
#include "PluginManagerStore.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SaveRasterWizard::SaveRasterWizard(const string& _model, QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), model(_model), features(0), openWhenFinished(false) {
    addPage(new AreaPage(this));
    addPage(new ContentsPage(this));
    addPage(new MetadataPage(this));
    addPage(new StatisticsPage(this));
    addPage(new DownloadPage(this));

    setOptions(NoBackButtonOnStartPage|NoBackButtonOnLastPage);
    setButtonText(CommitButton, tr("Download"));
    setWindowTitle(tr("Save map as..."));

    /* Save tile size */
    tileSize = MainWindow::instance()->rasterModelForRead()()->tileSize();
}

int SaveRasterWizard::exec() {
    /* Tile size and features of source model. Set ConvertableCoords feature
        only if the map has valid projection */
    Locker<const AbstractRasterModel> sourceModel = MainWindow::instance()->rasterModelForRead();
    int sourceFeatures = sourceModel()->features() & ~(sourceModel()->projection() ?
        0 : AbstractRasterModel::ConvertableCoords);
    sourceModel.unlock();

    /* Features of destination model */
    AbstractRasterModel* destinationModel = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager()->instance(model);
    if(!destinationModel) features = sourceFeatures; /* will fail in download page too, so don't bother */
    else {
        modelName = *destinationModel->metadata()->name();
        features = destinationModel->features();
        extensions = destinationModel->fileExtensions();
    }
    delete destinationModel;

    /* Check what features are missing in destination model */
    QString log;
    QString templ("<li><strong>%0</strong> - %1</li>");
    for(int i = 0; i != 32; ++i) {
        int feature = 1 << i;

        /* Check features that exists in source model and are not present in
           destination model */
        if((feature & sourceFeatures) && !(feature & features)) {
            switch(feature) {
                case AbstractRasterModel::ConvertableCoords:
                    log += templ.arg(tr("No GPS coordinates support")).arg(tr("you won't be able to measure distances.")); break;
            }
        }

        /* Check features that exists in destination model and are not present
           in source model */
        else if((feature & features) && !(feature & sourceFeatures)) {
            switch(feature) {
                case AbstractRasterModel::SingleLayer:
                    log += templ.arg(tr("No multi-layer support")).arg(tr("you can save only one layer."));
                    break;
                case AbstractRasterModel::SingleZoom:
                    log += templ.arg(tr("No multi-zoom support")).arg(tr("you can save only one zoom level."));
                    break;
            }
        }
    }

    if(!log.isEmpty()) {
        if(MessageBox::question(this, tr("Data loss warning"), tr("You are saving to a format which doesn't support all features provided by the map:<ul>%0</ul>Do you want to continue?").arg(log), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) return QWizard::Rejected;
    }

    return QWizard::exec();
}

TileArea SaveRasterWizard::area() const {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    TileArea area = rasterModel()->area()*pow2(zoomLevels[0]-*rasterModel()->zoomLevels().begin());

    /* Tile area at minimal zoom */
    TileArea ta;
    ta.x = area.x+absoluteArea.x1*area.w;
    ta.y = area.y+absoluteArea.y1*area.h;
    ta.w = area.x+absoluteArea.x2*area.w-ta.x+1;
    ta.h = area.y+absoluteArea.y2*area.h-ta.y+1;

    return ta;
}

void SaveRasterWizard::done(int result) {
    /* If cancelling unfinished download, display question messagebox */
    if(result == Rejected && currentId() == 4 && !currentPage()->isComplete() && MessageBox::question(this, tr("Download in progress"), tr("Package creation is in progress. Do you really want to cancel the operation?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;

    /* Open the package, if set */
    if(result == Accepted && openWhenFinished == true) {
        AbstractRasterModel* m = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager()->instance(model);

        if(m->addPackage(filename) == -1)
            delete m;
        else
            MainWindow::instance()->setRasterModel(m);
    }

    QWizard::done(result);
}

}}}
