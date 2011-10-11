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

#include "CacheTab.h"

#include <QtCore/QFileInfo>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentRun>
#include <QtGui/QComboBox>
#include <QtGui/QFileDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>

#include "MainWindow.h"
#include "MessageBox.h"
#include "PluginManager.h"
#include "PluginManagerStore.h"
#include "PluginModel.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

CacheTab::CacheTab(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f) {
    /* Cache model */
    pluginModel = MainWindow::instance()->pluginManagerStore()->caches()->loadedOnlyModel();

    /* Cache plugin */
    plugin = new QComboBox;
    plugin->setModel(pluginModel);
    plugin->setModelColumn(PluginModel::Name);

    /* Cache dir */
    dir = new QLineEdit;
    QToolButton* cacheDirButton = new QToolButton;
    cacheDirButton->setIcon(QIcon(":/open-16.png"));
    cacheDirButton->setAutoRaise(true);
    connect(cacheDirButton, SIGNAL(clicked(bool)), SLOT(selectCacheDir()));

    /* Cache size */
    size = new QSpinBox;
    size->setSuffix(" MB");
    size->setMinimum(0);
    size->setMaximum(4096);

    /* Cache block size */
    blockSize = new QSpinBox;
    blockSize->setSuffix(" B");
    blockSize->setMinimum(0);
    blockSize->setMaximum(33554432); /* 32 MB */

    /* Cache usage */
    usageLabel = new QLabel(tr("Used size:"));
    usage = new QProgressBar;
    usage->setMinimum(0);
    usage->setMaximum(100);

    /* Disable-able configuration */
    configurationGroup = new QGroupBox(tr("Use cache"));
    configurationGroup->setCheckable(true);

    /* Cache management buttons */
    optimizeButton = new QPushButton(tr("Optimize cache"));
    purgeButton = new QPushButton(tr("Purge cache"));
    connect(optimizeButton, SIGNAL(clicked(bool)), SLOT(optimize()));
    connect(purgeButton, SIGNAL(clicked(bool)), SLOT(purge()));

    /* Emit signal when edited */
    connect(configurationGroup, SIGNAL(toggled(bool)), SIGNAL(edited()));
    connect(plugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
    connect(dir, SIGNAL(textChanged(QString)), SIGNAL(edited()));
    connect(size, SIGNAL(valueChanged(int)), SIGNAL(edited()));
    connect(blockSize, SIGNAL(valueChanged(int)), SIGNAL(edited()));

    /* If the plugin dir or size is changed, reset cache size and usage fields
       to indicate that new cache is being created */
    connect(plugin, SIGNAL(currentIndexChanged(int)), SLOT(resetCacheSize()));
    connect(dir, SIGNAL(textChanged(QString)), SLOT(resetCacheSize()));

    /* Cache directory layout */
    QHBoxLayout* cacheDirLayout = new QHBoxLayout;
    cacheDirLayout->addWidget(dir);
    cacheDirLayout->addWidget(cacheDirButton);

    /* Form layout */
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Cache plugin:"), plugin);
    formLayout->addRow(tr("Cache dir:"), cacheDirLayout);
    formLayout->addRow(tr("Cache size:"), size);
    formLayout->addRow(tr("Cache block size:"), blockSize);

    /* Configuration layout */
    QGridLayout* configurationLayout = new QGridLayout;
    configurationLayout->addLayout(formLayout, 0, 0, 1, 2);
    configurationLayout->setRowStretch(0, 1);
    configurationLayout->addWidget(usageLabel, 1, 0, 1, 2);
    configurationLayout->addWidget(usage, 2, 0, 1, 2);
    configurationLayout->addWidget(optimizeButton, 3, 0);
    configurationLayout->addWidget(purgeButton, 3, 1);
    setLayout(configurationLayout);

    configurationGroup->setLayout(configurationLayout);

    /* Layout */
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(configurationGroup);
    setLayout(layout);

    /* Fill in values */
    reset();
}

void CacheTab::reset() {
    /* Disconnect edited() signals, so it will not enable save/apply buttons */
    /** @todo Handle this elsewhere? */
    disconnect(plugin, SIGNAL(currentIndexChanged(int)), this, SIGNAL(edited()));

    ConfigurationGroup* conf = MainWindow::instance()->configuration()->group("cache");

    configurationGroup->setChecked(conf->value<bool>("enabled"));
    plugin->setCurrentIndex(pluginModel->findPlugin(
    QString::fromStdString(conf->value<string>("plugin"))));
    dir->setText(QString::fromStdString(conf->value<string>("path")));

    resetCacheSize();

    /* Connect edited() back */
    connect(plugin, SIGNAL(currentIndexChanged(int)), SIGNAL(edited()));
}

void CacheTab::resetCacheSize() {
    ConfigurationGroup* conf = MainWindow::instance()->configuration()->group("cache");

    /* Disconnect edited() signals, so it will not enable save/apply buttons */
    /** @todo Handle this elsewhere? */
    disconnect(size, SIGNAL(valueChanged(int)), this, SIGNAL(edited()));
    disconnect(blockSize, SIGNAL(valueChanged(int)), this, SIGNAL(edited()));

    /* Reset cache size, cache block size and usage meter to values from
       current cache or default values from configuration, if no cache is set. */
    Locker<const AbstractCache> cache = MainWindow::instance()->cacheForRead();
    if(cache()) {
        size->setValue(cache()->cacheSize()/1024/1024);
        blockSize->setValue(cache()->blockSize());
        usage->setValue(cache()->usedSize()*100/cache()->cacheSize());
    } else {
        size->setValue(
            conf->value<unsigned int>("size"));
        blockSize->setValue(
            conf->value<unsigned int>("blockSize"));
        usage->setValue(0);
    }

    /* Connect edited() back */
    connect(size, SIGNAL(valueChanged(int)), SIGNAL(edited()));
    connect(blockSize, SIGNAL(valueChanged(int)), SIGNAL(edited()));
}

void CacheTab::restoreDefaults() {
    ConfigurationGroup* conf = MainWindow::instance()->configuration()->group("cache");

    string plugin = conf->value<string>("plugin");
    string path = conf->value<string>("path");

    conf->removeValue("enabled");
    conf->removeValue("plugin");
    conf->removeValue("path");
    conf->removeValue("size");
    conf->removeValue("blockSize");
    MainWindow::instance()->loadDefaultConfiguration();

    /* If the plugin/path was changed, initialize new cache */
    if(plugin != conf->value<string>("plugin") || path != conf->value<string>("path"))
        initialize();

    reset();
}

void CacheTab::save() {
    ConfigurationGroup* conf = MainWindow::instance()->configuration()->group("cache");

    conf->setValue<bool>("enabled", configurationGroup->isChecked());

    /* Modifying already initialized cache */
    if(MainWindow::instance()->cacheForRead()() && pluginModel->index(plugin->currentIndex(), PluginModel::Plugin).data().toString().toStdString() == conf->value<string>("plugin") && dir->text().toStdString() == conf->value<string>("path")) {
        QFutureWatcher<void>* blockSizeWatcher = 0;

        /* Modifying block size */
        if(MainWindow::instance()->cacheForRead()()->blockSize() != static_cast<size_t>(blockSize->value())) {
            startBlockingOperation(tr("Setting block size to %0 B...").arg(blockSize->value()));
            QFuture<void> future = QtConcurrent::run(this, &CacheTab::setBlockSizeInternal, blockSize->value());
            blockSizeWatcher = new QFutureWatcher<void>(this);
            blockSizeWatcher->setFuture(future);
        }

        /* Modifying cache size */
        if(MainWindow::instance()->cacheForRead()()->cacheSize() != static_cast<size_t>(size->value()*1024*1024)) {
            /* If modifying also block size, wait for it to finish and then set block size */
            if(blockSizeWatcher)
                connect(blockSizeWatcher, SIGNAL(finished()), this, SLOT(setSize()));
            else setSize();

        /* If setting only block size, finish blocking operation after it is done */
        } else if(blockSizeWatcher)
            connect(blockSizeWatcher, SIGNAL(finished()), SLOT(finishBlockingOperation()));

    /* Initializing new cache */
    } else {
        conf->setValue<string>("plugin",
            pluginModel->index(plugin->currentIndex(), PluginModel::Plugin).data().toString().toStdString());
        conf->setValue<string>("path", dir->text().toStdString());
        conf->setValue<unsigned int>("size", size->value());
        conf->setValue<unsigned int>("blockSize", blockSize->value());

        initialize();
    }
}

void CacheTab::initialize() {
    ConfigurationGroup* conf = MainWindow::instance()->configuration()->group("cache");

    /* Call asynchronous initialization and block until it is ready */
    AbstractCache* cache = 0;
    if(conf->value<bool>("enabled"))
        cache = MainWindow::instance()->pluginManagerStore()->caches()->manager()->instance(conf->value<string>("plugin"));
    MainWindow::instance()->setCache(cache);

    startBlockingOperation(tr("Initializing cache..."));
    QFuture<void> future = QtConcurrent::run(this, &CacheTab::initializeInternal);
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), SLOT(finishBlockingOperation()));
}

void CacheTab::selectCacheDir() {
    QString selected = QFileDialog::getExistingDirectory(this, tr("Select cache directory"), dir->text());
    if(selected.isEmpty()) return;

    QDir d(QFileInfo(selected).absoluteDir());
    QStringList l = d.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);

    /* If the dir contains files, show warning */
    if(l.count() > 1 && MessageBox::question(this, tr("Non-empty directory"), tr("The directory contains some files which will be overwritten with the cache. Do you want to continue?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    dir->setText(selected);
}

void CacheTab::setSize() {
    startBlockingOperation(tr("Setting cache size to %0 MB...").arg(size->value()));
    QFuture<void> future = QtConcurrent::run(this, &CacheTab::setSizeInternal, size->value());
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), SLOT(finishBlockingOperation()));
}

void CacheTab::optimize() {
    startBlockingOperation(tr("Optimizing cache..."));
    QFuture<void> future = QtConcurrent::run(this, &CacheTab::optimizeInternal);
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), SLOT(finishBlockingOperation()));
}

void CacheTab::purge() {
    if(MessageBox::question(this, tr("Cache purge"), tr("Are you sure you want to remove all items from the cache?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

    startBlockingOperation(tr("Purging cache..."));
    QFuture<void> future = QtConcurrent::run(this, &CacheTab::purgeInternal);
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), SLOT(finishBlockingOperation()));
}

void CacheTab::initializeInternal() {
    /* Wait for the initialization thread in MainWindow to start */
    usleep(100000);

    /* Block until the cache is initialized */
    MainWindow::instance()->cacheForRead()();
}

void CacheTab::setSizeInternal(size_t size) {
    MainWindow::instance()->cacheForWrite()()->setCacheSize(size*1024*1024);
}

void CacheTab::setBlockSizeInternal(size_t size) {
    MainWindow::instance()->cacheForWrite()()->setBlockSize(size);
}

void CacheTab::optimizeInternal() {
    MainWindow::instance()->cacheForWrite()()->optimize();
}

void CacheTab::purgeInternal() {
    MainWindow::instance()->cacheForWrite()()->purge();
}

void CacheTab::startBlockingOperation(const QString& description) {
    usage->setMaximum(0);
    usageLabel->setText(description);

    plugin->setDisabled(true);
    dir->setDisabled(true);
    size->setDisabled(true);
    blockSize->setDisabled(true);
    optimizeButton->setDisabled(true);
    purgeButton->setDisabled(true);

    emit blockingOperation(true);
}

void CacheTab::finishBlockingOperation() {
    usage->setMaximum(100);
    usageLabel->setText(tr("Used size:"));

    plugin->setDisabled(false);
    dir->setDisabled(false);
    size->setDisabled(false);
    blockSize->setDisabled(false);
    optimizeButton->setDisabled(false);
    purgeButton->setDisabled(false);

    resetCacheSize();

    emit blockingOperation(false);
}

}}}
