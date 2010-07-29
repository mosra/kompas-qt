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

#include "PluginDialogTab.h"

#include <QtGui/QTableView>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDataWidgetMapper>
#include <QtGui/QHeaderView>

#include "PluginManager/AbstractPluginManager.h"
#include "MainWindow.h"
#include "PluginModel.h"

using namespace std;

namespace Map2X { namespace QtGui {

PluginDialogTab::PluginDialogTab(MainWindow* _mainWindow, const std::string& _configurationKey, PluginManager::AbstractPluginManager* _manager, const QString& _categoryDescription, QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f), mainWindow(_mainWindow), configurationKey(_configurationKey), manager(_manager) {
    string _pluginDir = _manager->pluginDirectory();
    mainWindow->configuration()->group("pluginDirs")->value<string>(_configurationKey, &_pluginDir);

    /* Initialize labels */
    pluginDir = new QLineEdit(QString::fromStdString(_pluginDir));
    QLabel* categoryDescription = new QLabel(_categoryDescription);
    categoryDescription->setWordWrap(true);
    loadState = new QLabel;
    description = new QLabel;
    description->setWordWrap(true);
    depends = new QLabel;
    depends->setWordWrap(true);
    replaces = new QLabel;
    replaces->setWordWrap(true);
    conflicts = new QLabel;
    conflicts->setWordWrap(true);

    /* Initialize model and pass it to view */
    model = new PluginModel(manager, 0, this);
    QTableView* view = new QTableView(this);
    view->verticalHeader()->setDefaultSectionSize(20);
    view->setModel(model);
    view->setColumnHidden(PluginModel::LoadState, true);
    view->setColumnHidden(PluginModel::Description, true);
    view->setColumnHidden(PluginModel::Depends, true);
    view->setColumnHidden(PluginModel::Replaces, true);
    view->setColumnHidden(PluginModel::Conflicts, true);
    view->setColumnWidth(PluginModel::CheckState, 30);
    view->setColumnWidth(PluginModel::Plugin, 175);
    view->setColumnWidth(PluginModel::Name, 250);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setShowGrid(false);

    /* Map data to widgets */
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(loadState, PluginModel::LoadState, "text");
    mapper->addMapping(description, PluginModel::Description, "text");
    mapper->addMapping(depends, PluginModel::Depends, "text");
    mapper->addMapping(replaces, PluginModel::Replaces, "text");
    mapper->addMapping(conflicts, PluginModel::Conflicts, "text");

    /* On selection change load new row in mapper */
    connect(view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    /* Emit signal on edit */
    connect(pluginDir, SIGNAL(textEdited(QString)), this, SIGNAL(edited()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(edited()));

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(categoryDescription, 0, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Plugin directory:")), 1, 0);
    layout->addWidget(pluginDir, 1, 1);
    layout->addWidget(view, 2, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Load state:")), 3, 0, Qt::AlignTop);
    layout->addWidget(loadState, 3, 1);
    layout->addWidget(new QLabel(tr("Description:")), 4, 0, Qt::AlignTop);
    layout->addWidget(description, 4, 1);
    layout->addWidget(new QLabel(tr("Depends on:")), 5, 0, Qt::AlignTop);
    layout->addWidget(depends, 5, 1);
    layout->addWidget(new QLabel(tr("Replaces:")), 6, 0, Qt::AlignTop);
    layout->addWidget(replaces, 6, 1);
    layout->addWidget(new QLabel(tr("Conflicts with:")), 7, 0, Qt::AlignTop);
    layout->addWidget(conflicts, 7, 1);
    layout->setRowStretch(2, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);
}

void PluginDialogTab::save() {
    /* If configuration dir is changed, change it */
    string currentPluginDir = pluginDir->text().toStdString();
    if(manager->pluginDirectory() != currentPluginDir)
        mainWindow->configuration()->group("pluginDirs")->setValue<string>(configurationKey, pluginDir->text().toStdString());
}

void PluginDialogTab::reset() {
    string _pluginDir;
    mainWindow->configuration()->group("pluginDirs")->value<string>(configurationKey, &_pluginDir);
    pluginDir->setText(QString::fromStdString(_pluginDir));
}

void PluginDialogTab::restoreDefaults() {
    /* Remove current pluginDir value from configuration and set it from defaults */
    mainWindow->configuration()->group("pluginDirs")->removeValue(configurationKey);
    mainWindow->loadDefaultConfiguration();

    /* Load the value from configuration */
    reset();
}

}}
