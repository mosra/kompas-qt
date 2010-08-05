/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2010 Jan Dupal <dupal.j@seznam.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "DistanceMeterToolDialog.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QMessageBox>

#include "../../Wgs84CoordsEdit.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui { namespace Plugins {

DistanceMeterToolDialog::DistanceMeterToolDialog(MainWindow* _mainWindow, QWidget* parent, Qt::WindowFlags f): AbstractToolDialog(_mainWindow, parent, f) {
    /* Initialize labels */
    coordsA = new Wgs84CoordsEdit;
    coordsB = new Wgs84CoordsEdit;
    distance = new QDoubleSpinBox;
    distance->setDecimals(3);
    distance->setReadOnly(true);
    distance->setSuffix(" m");
    distance->setMaximum(100000000.0);

    /* Buttons */
    QPushButton* calculateButton = new QPushButton(tr("Calculate"));
    connect(calculateButton, SIGNAL(clicked(bool)), SLOT(calculate()));

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Place A:")), 0, 0);
    layout->addWidget(coordsA, 0, 1);
    layout->addWidget(new QLabel(tr("Place B:")), 1, 0);
    layout->addWidget(coordsB, 1, 1);
    layout->addWidget(new QLabel(tr("Distance:")), 2, 0);
    layout->addWidget(distance, 2, 1);
    layout->addWidget(calculateButton, 3, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);

    setWindowTitle(tr("Distance meter"));
    setMinimumWidth(320);
}

void DistanceMeterToolDialog::calculate() {
    double _distance = Wgs84Coords::distance(coordsA->coords(), coordsB->coords());

    if(_distance < 0) {
        QMessageBox information(this);
        information.setIcon(QMessageBox::Information);
        information.setWindowTitle(tr("Mosra & Co. Travel Agency"));
        information.setText(tr("Congratulations! You have chosen a trip around the world."));
        information.setInformativeText(tr("Please choose at least one transfer "
            "point so we can compute the most accurate travel distance for you."));
        information.exec();
    }

    distance->setValue(_distance);
}

}}}