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

#include "ConfigurationWidget.h"

#include <string>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>

#include "PluginModel.h"
#include "PluginManagerStore.h"
#include "MainWindow.h"
#include "MessageBox.h"

using namespace std;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

ConfigurationWidget::ConfigurationWidget(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f) {
    /* Map view model */
    mapViewModel = MainWindow::instance()->pluginManagerStore()->mapViews()->loadedOnlyModel();

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

    /* Cache model */
    cacheModel = MainWindow::instance()->pluginManagerStore()->caches()->loadedOnlyModel();

    /* Cache plugin */
    cachePlugin = new QComboBox;
    cachePlugin->setModel(cacheModel);
    cachePlugin->setModelColumn(PluginModel::Name);

    /* Cache dir */
    cacheDir = new QLineEdit;
    QToolButton* cacheDirButton = new QToolButton;
    cacheDirButton->setIcon(QIcon(":/open-16.png"));
    cacheDirButton->setAutoRaise(true);
    connect(cacheDirButton, SIGNAL(clicked(bool)), SLOT(selectCacheDir()));

    /* Cache size */
    cacheSize = new QSpinBox;
    cacheSize->setSuffix(" MB");
    cacheSize->setMinimum(0);
    cacheSize->setMaximum(4096);

    /* Cache block size */
    cacheBlockSize = new QSpinBox;
    cacheBlockSize->setSuffix(" B");
    cacheBlockSize->setMinimum(0);
    cacheBlockSize->setMaximum(33554432); /* 32 MB */

    /* Emit signal when edited */
    connect(mapViewPlugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
    connect(maxSimultaenousDownloads, SIGNAL(valueChanged(int)), SIGNAL(edited()));
    connect(packageDir, SIGNAL(textChanged(QString)), SIGNAL(edited()));
    connect(cachePlugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
    connect(cacheDir, SIGNAL(textChanged(QString)), SIGNAL(edited()));
    connect(cacheSize, SIGNAL(valueChanged(int)), SIGNAL(edited()));
    connect(cacheBlockSize, SIGNAL(valueChanged(int)), SIGNAL(edited()));

    /* Package directory layout */
    QHBoxLayout* packageDirLayout = new QHBoxLayout;
    packageDirLayout->addWidget(packageDir);
    packageDirLayout->addWidget(packageDirButton);

    /* Cache directory layout */
    QHBoxLayout* cacheDirLayout = new QHBoxLayout;
    cacheDirLayout->addWidget(cacheDir);
    cacheDirLayout->addWidget(cacheDirButton);

    /* Layout */
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Map view plugin:"), mapViewPlugin);
    layout->addRow(tr("Max simultaenous downloads:"), maxSimultaenousDownloads);
    layout->addRow(tr("Map package directory:"), packageDirLayout);
    layout->addRow(tr("Cache plugin:"), cachePlugin);
    layout->addRow(tr("Cache dir:"), cacheDirLayout);
    layout->addRow(tr("Cache size:"), cacheSize);
    layout->addRow(tr("Cache block size:"), cacheBlockSize);
    setLayout(layout);

    /* Fill in values */
    reset();
}

void ConfigurationWidget::reset() {
    mapViewPlugin->setCurrentIndex(mapViewModel->findPlugin(QString::fromStdString(
        MainWindow::instance()->configuration()->group("map")->value<string>("viewPlugin"))));
    maxSimultaenousDownloads->setValue(
        MainWindow::instance()->configuration()->group("map")->value<int>("maxSimultaenousDownloads"));
    packageDir->setText(QString::fromStdString(
        MainWindow::instance()->configuration()->group("paths")->value<string>("packages")));
    cachePlugin->setCurrentIndex(cacheModel->findPlugin(QString::fromStdString(
        MainWindow::instance()->configuration()->group("cache")->value<string>("plugin"))));
    cacheDir->setText(QString::fromStdString(
        MainWindow::instance()->configuration()->group("cache")->value<string>("path")));
    cacheSize->setValue(
        MainWindow::instance()->configuration()->group("cache")->value<unsigned int>("size"));
    cacheBlockSize->setValue(
        MainWindow::instance()->configuration()->group("cache")->value<unsigned int>("blockSize"));
}

void ConfigurationWidget::restoreDefaults() {
    MainWindow::instance()->configuration()->group("map")->removeValue("viewPlugin");
    MainWindow::instance()->configuration()->group("map")->removeValue("maxSimultaenousDownloads");
    MainWindow::instance()->configuration()->group("paths")->removeValue("packages");
    MainWindow::instance()->configuration()->group("cache")->removeValue("plugin");
    MainWindow::instance()->configuration()->group("cache")->removeValue("path");
    MainWindow::instance()->configuration()->group("cache")->removeValue("size");
    MainWindow::instance()->configuration()->group("cache")->removeValue("blockSize");
    MainWindow::instance()->loadDefaultConfiguration();

    reset();
}

void ConfigurationWidget::save() {
    MainWindow::instance()->configuration()->group("map")->setValue<string>("viewPlugin",
        mapViewModel->index(mapViewPlugin->currentIndex(), PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->configuration()->group("map")->setValue<int>("maxSimultaenousDownloads",
        maxSimultaenousDownloads->value());
    MainWindow::instance()->configuration()->group("paths")->setValue<string>("packages",
        packageDir->text().toStdString());
    MainWindow::instance()->configuration()->group("cache")->setValue<string>("plugin",
        cacheModel->index(cachePlugin->currentIndex(), PluginModel::Plugin).data().toString().toStdString());
    MainWindow::instance()->configuration()->group("cache")->setValue<string>("path",
        cacheDir->text().toStdString());
    MainWindow::instance()->configuration()->group("cache")->setValue<unsigned int>("size",
        cacheSize->value());
    MainWindow::instance()->configuration()->group("cache")->setValue<unsigned int>("blockSize",
        cacheBlockSize->value());
}

void ConfigurationWidget::selectPackageDir() {
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select package directory"), packageDir->text());
    if(selected.isEmpty()) return;

    packageDir->setText(selected);
}

void ConfigurationWidget::selectCacheDir() {
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select cache directory"), cacheDir->text());
    if(selected.isEmpty()) return;

    QDir d(QFileInfo(selected).absoluteDir());
    QStringList l = d.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);

    /* If the dir contains files, show warning */
    if(l.count() > 1 && MessageBox::question(this, tr("Non-empty directory"), tr("The directory contains some files which will be overwritten with the cache. Do you want to continue?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    cacheDir->setText(selected);
}

}}}
