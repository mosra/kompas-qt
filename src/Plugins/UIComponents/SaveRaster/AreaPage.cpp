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

#include "AreaPage.h"

#include <QtGui/QGridLayout>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "SaveRasterWizard.h"
#include "MainWindow.h"

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

AreaPage::AreaPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("1/5: Map area"));
    setSubTitle(tr("Select map area which you want to save."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/progress1-48.png"));

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

bool AreaPage::validatePage() {
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
        /** @todo @c VERSION-0.2 CustomArea to be implemented... */
    }

    return true;
}

}}}
