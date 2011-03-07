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

#include "DistanceMeterDialog.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QComboBox>

#include "LatLonCoordsEdit.h"
#include "MessageBox.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "PluginManager.h"
#include "CurrentCelestialBodyPluginModel.h"

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

DistanceMeterDialog::DistanceMeterDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f) {
    /* Celestial bodies plugin model */
    CurrentCelestialBodyPluginModel* proxyModel = new CurrentCelestialBodyPluginModel(this);
    proxyModel->setSourceModel(MainWindow::instance()->pluginManagerStore()->celestialBodies()->loadedOnlyModel());

    /* Initialize labels */
    celestialBody = new QComboBox;
    celestialBody->setModel(proxyModel);
    celestialBody->setModelColumn(PluginModel::Name);
    coordsA = new LatLonCoordsEdit;
    coordsB = new LatLonCoordsEdit;
    distance = new QDoubleSpinBox;
    distance->setDecimals(4);
    distance->setReadOnly(true);
    distance->setSuffix(" km");
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
    setAttribute(Qt::WA_DeleteOnClose);

    /* Set index to current model */
    int row = -1;
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    if(rasterModel()) row = MainWindow::instance()->pluginManagerStore()->celestialBodies()->loadedOnlyModel()->findPlugin(QString::fromStdString(rasterModel()->celestialBody()));
    rasterModel.unlock();

    if(row != -1) celestialBody->setCurrentIndex(row);
}

void DistanceMeterDialog::calculate() {
    double _distance = 0;

    /* Celestial body instance */
    string plugin = celestialBody->model()->index(celestialBody->currentIndex(), PluginModel::Plugin).data().toString().toStdString();

    AbstractCelestialBody* body = MainWindow::instance()->pluginManagerStore()->celestialBodies()->manager()->instance(plugin);
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

    distance->setValue(_distance/1000);
}

}}}
