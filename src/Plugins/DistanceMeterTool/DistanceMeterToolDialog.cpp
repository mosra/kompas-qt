/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2010 Jan Dupal <dupal.j@seznam.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "DistanceMeterToolDialog.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QComboBox>

#include "Wgs84CoordsEdit.h"
#include "MessageBox.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "PluginManager.h"

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins {

DistanceMeterToolDialog::DistanceMeterToolDialog(const AbstractTool* _tool, QWidget* parent, Qt::WindowFlags f): AbstractToolDialog(_tool, parent, f) {
    /* Celestial bodies plugin model */
    PluginModel* model = new PluginModel(MainWindow::instance()->celestialBodyPluginManager(), PluginModel::LoadedOnly, this);

    /* Initialize labels */
    celestialBody = new QComboBox;
    celestialBody->setModel(model);
    celestialBody->setModelColumn(PluginModel::Name);
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
    layout->addWidget(new QLabel(tr("Celestial body:"), 0, 0));
    layout->addWidget(celestialBody, 0, 1);
    layout->addWidget(new QLabel(tr("Place A:")), 1, 0);
    layout->addWidget(coordsA, 1, 1);
    layout->addWidget(new QLabel(tr("Place B:")), 2, 0);
    layout->addWidget(coordsB, 2, 1);
    layout->addWidget(new QLabel(tr("Distance:")), 3, 0);
    layout->addWidget(distance, 3, 1);
    layout->addWidget(calculateButton, 4, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);

    setMinimumWidth(320);
}

void DistanceMeterToolDialog::calculate() {
    double _distance = 0;

    /* Celestial body instance */
    string plugin = celestialBody->model()->index(celestialBody->currentIndex(), PluginModel::Plugin).data().toString().toStdString();

    AbstractCelestialBody* body = MainWindow::instance()->celestialBodyPluginManager()->instance(plugin);
    if(body) {
        _distance = body->distance(coordsA->coords(), coordsB->coords());
        delete body;
    }

    if(_distance < 0) {
        MessageBox information(this);
        information.setIcon(QMessageBox::Information);
        information.setWindowTitle(tr("Mosra & Co. Travel Agency"));
        information.setText(tr("Congratulations! You have chosen a trip around the world."));
        information.setInformativeText(tr("Please choose at least one transfer "
            "point so we can compute the most accurate travel distance for you."));
        information.exec();
    }

    distance->setValue(_distance);
}

}}
