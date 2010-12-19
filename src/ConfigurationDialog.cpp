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

#include "ConfigurationDialog.h"

#include <string>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>

#include "PluginManager.h"
#include "PluginModel.h"
#include "MainWindow.h"

using namespace std;

namespace Kompas { namespace QtGui {

ConfigurationDialog::ConfigurationDialog(MainWindow* mainWindow, Qt::WindowFlags f): AbstractConfigurationDialog(mainWindow, f) {
    Widget* widget = new Widget(this);
    connectWidget(widget);
    setCentralWidget(widget);
    setWindowTitle(tr("Kompas configuration"));
    resize(480, 240);
}

ConfigurationDialog::Widget::Widget(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f) {
    /* Map view model */
    mapViewModel = new PluginModel(MainWindow::instance()->mapViewPluginManager(), PluginModel::LoadedOnly, this);

    /* Map view plugin */
    mapViewPlugin = new QComboBox;
    mapViewPlugin->setModel(mapViewModel);
    mapViewPlugin->setModelColumn(PluginModel::Name);

    /* Maximal count of simultaenous downloads */
    maxSimultaenousDownloads = new QSpinBox;
    maxSimultaenousDownloads->setMinimum(1);
    maxSimultaenousDownloads->setMaximum(5);

    /* Package directory with selecting button */
    packageDir = new QLineEdit;
    QToolButton* packageDirButton = new QToolButton;
    packageDirButton->setIcon(QIcon(":/open-16.png"));
    packageDirButton->setAutoRaise(true);
    connect(packageDirButton, SIGNAL(clicked(bool)), SLOT(selectPackageDir()));

    /* Emit signal when edited */
    connect(mapViewPlugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
    connect(maxSimultaenousDownloads, SIGNAL(valueChanged(int)), SIGNAL(edited()));
    connect(packageDir, SIGNAL(textChanged(QString)), SIGNAL(edited()));

    /* Package directory layout */
    QHBoxLayout* packageDirLayout = new QHBoxLayout;
    packageDirLayout->addWidget(packageDir);
    packageDirLayout->addWidget(packageDirButton);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Map view plugin:")), 0, 0);
    layout->addWidget(mapViewPlugin, 0, 1);
    layout->addWidget(new QLabel(tr("Max simultaenous downloads:")), 1, 0);
    layout->addWidget(maxSimultaenousDownloads, 1, 1);
    layout->addWidget(new QLabel(tr("Map package directory:")), 2, 0);
    layout->addLayout(packageDirLayout, 2, 1);
    layout->addWidget(new QWidget, 3, 0, 1, 2);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(4, 1);
    setLayout(layout);

    /* Fill in values */
    reset();
}

void ConfigurationDialog::Widget::reset() {
    mapViewPlugin->setCurrentIndex(mapViewModel->findPlugin(QString::fromStdString(
        MainWindow::instance()->configuration()->group("map")->value<string>("viewPlugin"))));
    maxSimultaenousDownloads->setValue(
        MainWindow::instance()->configuration()->group("map")->value<int>("maxSimultaenousDownloads"));
    packageDir->setText(QString::fromStdString(
        MainWindow::instance()->configuration()->group("paths")->value<string>("packages")));
}

void ConfigurationDialog::Widget::restoreDefaults() {
    MainWindow::instance()->configuration()->group("map")->removeValue("viewPlugin");
    MainWindow::instance()->configuration()->group("map")->removeValue("maxSimultaenousDownloads");
    MainWindow::instance()->configuration()->group("paths")->removeValue("packages");
    MainWindow::instance()->loadDefaultConfiguration();

    reset();
}

void ConfigurationDialog::Widget::save() {
    MainWindow::instance()->configuration()->group("map")->setValue<string>("viewPlugin",
        mapViewModel->index(mapViewPlugin->currentIndex(), PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->configuration()->group("map")->setValue<int>("maxSimultaenousDownloads",
        maxSimultaenousDownloads->value());
    MainWindow::instance()->configuration()->group("paths")->setValue<string>("packages",
        packageDir->text().toStdString());
}

void ConfigurationDialog::Widget::selectPackageDir() {
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select package directory"), packageDir->text());
    if(selected.isEmpty()) return;

    packageDir->setText(selected);
}

}}
