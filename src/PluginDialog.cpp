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

#include "PluginDialog.h"

#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDataWidgetMapper>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>

#include "PluginManager.h"
#include "MainWindow.h"
#include "PluginModel.h"

using namespace std;

namespace Map2X { namespace QtGui {

PluginDialog::PluginDialog(MainWindow* mainWindow, Qt::WindowFlags f): AbstractConfigurationDialog(mainWindow, f) {
    /* Tabs */
    tabs = new QTabWidget;
    Tab* mapViewTab = new Tab(
        mainWindow,
        "mapViews",
        mainWindow->mapViewPluginManager(),
        tr("Plugins providing map view area."));
    tabs->addTab(mapViewTab, tr("Map viewers"));
    connectWidget(mapViewTab);

    Tab* projectionTab = new Tab(
        mainWindow,
        "projections",
        mainWindow->projectionPluginManager(),
        tr("Plugins for map projections."));
    tabs->addTab(projectionTab, tr("Projections"));
    connectWidget(projectionTab);

    Tab* rasterModelTab = new Tab(
        mainWindow,
        "rasterModels",
        mainWindow->rasterModelPluginManager(),
        tr("Plugins for displaying different kinds of raster maps."));
    tabs->addTab(rasterModelTab, tr("Raster maps"));
    connectWidget(rasterModelTab);

    Tab* toolsTab = new Tab(
        mainWindow,
        "tools",
        mainWindow->toolPluginManager(),
        tr("Various utilites for data computing and conversion."));
    tabs->addTab(toolsTab, tr("Tools"));
    connectWidget(toolsTab);

    setCentralWidget(tabs);
    setWindowTitle("Plugins");
    resize(640, 480);
}

PluginDialog::Tab::Tab(MainWindow* _mainWindow, const std::string& _configurationKey, AbstractPluginManager* _manager, const QString& _categoryDescription, QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f), mainWindow(_mainWindow), configurationKey(_configurationKey), manager(_manager) {
    /* Initialize labels */
    pluginDir = new QLineEdit;
    QLabel* categoryDescription = new QLabel(_categoryDescription);
    categoryDescription->setWordWrap(true);
    loadState = new QLabel;
    description = new QLabel;
    description->setWordWrap(true);
    authors = new QLabel;
    authors->setWordWrap(true);
    depends = new QLabel;
    depends->setWordWrap(true);
    usedBy = new QLabel;
    usedBy->setWordWrap(true);
    replaces = new QLabel;
    replaces->setWordWrap(true);
    replacedWith = new QLabel;
    replacedWith->setWordWrap(true);

    /* Button for selecting plugin dir */
    QPushButton* pluginDirButton = new QPushButton(style()->standardIcon(QStyle::SP_DirOpenIcon), tr("Select..."));
    connect(pluginDirButton, SIGNAL(clicked(bool)), SLOT(setPluginDir()));

    /* Initialize model and pass it to view */
    model = new PluginModel(manager, 0, this);
    QTableView* view = new QTableView(this);
    view->verticalHeader()->setDefaultSectionSize(20);
    view->setModel(model);
    view->setColumnHidden(PluginModel::Description, true);
    view->setColumnHidden(PluginModel::Authors, true);
    view->setColumnHidden(PluginModel::Depends, true);
    view->setColumnHidden(PluginModel::UsedBy, true);
    view->setColumnHidden(PluginModel::Replaces, true);
    view->setColumnHidden(PluginModel::ReplacedWith, true);
    view->setColumnWidth(PluginModel::LoadState, 125);
    view->setColumnWidth(PluginModel::Plugin, 150);
    view->setColumnWidth(PluginModel::Version, 50);
    view->setColumnWidth(PluginModel::Name, 225);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setShowGrid(false);

    view->horizontalHeader()->moveSection(PluginModel::Version, PluginModel::Name);

    /* Map data to widgets */
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(loadState, PluginModel::LoadState, "text");
    mapper->addMapping(description, PluginModel::Description, "text");
    mapper->addMapping(authors, PluginModel::Authors, "text");
    mapper->addMapping(depends, PluginModel::Depends, "text");
    mapper->addMapping(usedBy, PluginModel::UsedBy, "text");
    mapper->addMapping(replaces, PluginModel::Replaces, "text");
    mapper->addMapping(replacedWith, PluginModel::ReplacedWith, "text");

    /* On selection change load new row in mapper */
    connect(view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    /* Emit signal on edit */
    connect(pluginDir, SIGNAL(textEdited(QString)), this, SIGNAL(edited()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(edited()));

    /* Layout for plugin dir lineedit and button */
    QHBoxLayout* pluginDirLayout = new QHBoxLayout;
    pluginDirLayout->addWidget(pluginDir, 1);
    pluginDirLayout->addWidget(pluginDirButton);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(categoryDescription, 0, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Plugin directory:")), 1, 0);
    layout->addLayout(pluginDirLayout, 1, 1);
    layout->addWidget(view, 2, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Load state:")), 3, 0, Qt::AlignTop);
    layout->addWidget(loadState, 3, 1);
    layout->addWidget(new QLabel(tr("Description:")), 4, 0, Qt::AlignTop);
    layout->addWidget(description, 4, 1);
    layout->addWidget(new QLabel(tr("Author(s):")), 5, 0, Qt::AlignTop);
    layout->addWidget(authors, 5, 1);
    layout->addWidget(new QLabel(tr("Depends on:")), 6, 0, Qt::AlignTop);
    layout->addWidget(depends, 6, 1);
    layout->addWidget(new QLabel(tr("Used by:")), 7, 0, Qt::AlignTop);
    layout->addWidget(usedBy, 7, 1);
    layout->addWidget(new QLabel(tr("Replaces:")), 8, 0, Qt::AlignTop);
    layout->addWidget(replaces, 8, 1);
    layout->addWidget(new QLabel(tr("Can be replaced with:")), 9, 0, Qt::AlignTop);
    layout->addWidget(replacedWith, 9, 1);
    layout->setRowStretch(2, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);

    /* Fill in values */
    reset();
}

void PluginDialog::Tab::save() {
    mainWindow->configuration()->group("plugins")->group(configurationKey)->setValue<string>("__dir", pluginDir->text().toStdString());

    for(int i = 0; i != model->rowCount(); ++i) {
        mainWindow->configuration()->group("plugins")->group(configurationKey)->setValue<bool>(
            model->index(i, PluginModel::Plugin).data().toString().toStdString(),
            model->index(i, PluginModel::LoadState).data(Qt::CheckStateRole).toBool());
    }
}

void PluginDialog::Tab::reset() {
    pluginDir->setText(QString::fromStdString(
        mainWindow->configuration()->group("plugins")->group(configurationKey)->value<string>("__dir")));
}

void PluginDialog::Tab::restoreDefaults() {
    /* Remove current pluginDir value from configuration and set it from defaults */
    mainWindow->configuration()->group("plugins")->removeGroup(configurationKey);
    mainWindow->loadDefaultConfiguration();

    /* Load the value from configuration */
    reset();
}

void PluginDialog::Tab::setPluginDir() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select plugin dir"), pluginDir->text());

    if(!dir.isEmpty()) {
        pluginDir->setText(dir);
        emit edited();
    }
}

}}
