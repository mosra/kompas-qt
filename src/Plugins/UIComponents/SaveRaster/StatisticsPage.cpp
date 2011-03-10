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

#include "StatisticsPage.h"

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "AbstractRasterModel.h"
#include "SaveRasterWizard.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace Plugins { namespace UIComponents {

StatisticsPage::StatisticsPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard), canDownload(true) {
    setTitle(tr("4/5: Statistics"));
    setSubTitle(tr("Review amount of data to be downloaded, return back and make changes or proceed to creating the package."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/progress4-48.png"));
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

void StatisticsPage::initializePage() {
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

}}}
