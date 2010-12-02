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

#include "ConfigurationWidget.h"

#include <string>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>

#include "PluginManager.h"
#include "PluginModel.h"
#include "MainWindow.h"
#include "Wgs84CoordsEdit.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

ConfigurationWidget::ConfigurationWidget(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f) {
    /* Map view plugin */
    mapViewPlugin = new QComboBox;
    mapViewPlugin->setModel(new PluginModel(
        MainWindow::instance()->mapViewPluginManager(), PluginModel::LoadedOnly, this));
    mapViewPlugin->setModelColumn(PluginModel::Plugin);

    /* Maximal count of simultaenous downloads */
    maxSimultaenousDownloads = new QSpinBox;
    maxSimultaenousDownloads->setMinimum(1);
    maxSimultaenousDownloads->setMaximum(5);

    /* Emit signal when edited */
    connect(mapViewPlugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
    connect(maxSimultaenousDownloads, SIGNAL(valueChanged(int)), SIGNAL(edited()));

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Map view plugin:")), 0, 0);
    layout->addWidget(mapViewPlugin, 0, 1);
    layout->addWidget(new QLabel(tr("Max simultaenous downloads:")), 1, 0);
    layout->addWidget(maxSimultaenousDownloads, 1, 1);
    layout->addWidget(new QWidget, 4, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(4, 1);
    setLayout(layout);

    /* Fill in values */
    reset();
}

void ConfigurationWidget::reset() {
    mapViewPlugin->setCurrentIndex(mapViewPlugin->findText(QString::fromStdString(
        MainWindow::instance()->configuration()->group("map")->value<string>("viewPlugin"))));
    maxSimultaenousDownloads->setValue(
        MainWindow::instance()->configuration()->group("map")->value<int>("maxSimultaenousDownloads"));
}

void ConfigurationWidget::restoreDefaults() {
    MainWindow::instance()->configuration()->group("map")->removeValue("viewPlugin");
    MainWindow::instance()->configuration()->group("map")->removeValue("maxSimultaenousDownloads");
    MainWindow::instance()->loadDefaultConfiguration();

    reset();
}

void ConfigurationWidget::save() {
    MainWindow::instance()->configuration()->group("map")->setValue<string>("viewPlugin",
        mapViewPlugin->currentText().toStdString());
    MainWindow::instance()->configuration()->group("map")->setValue<int>("maxSimultaenousDownloads",
        maxSimultaenousDownloads->value());
}

}}
