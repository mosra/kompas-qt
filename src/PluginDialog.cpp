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
#include "MessageBox.h"

using namespace std;

namespace Kompas { namespace QtGui {

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
    resize(640, 400);
    setAttribute(Qt::WA_DeleteOnClose);
}

PluginDialog::Tab::Tab(MainWindow* _mainWindow, const std::string& _configurationKey, AbstractPluginManager* _manager, const QString& _categoryDescription, QWidget* parent, Qt::WindowFlags f): AbstractConfigurationWidget(parent, f), mainWindow(_mainWindow), configurationKey(_configurationKey), manager(_manager) {
    /* Initialize labels */
    pluginDir = new QLineEdit;
    QLabel* categoryDescription = new QLabel(_categoryDescription);
    categoryDescription->setWordWrap(true);
    loadStateLabel = new QLabel(tr("Load state:"));
    loadStateLabel->setHidden(true);
    loadState = new QLabel;
    loadState->setHidden(true);
    descriptionLabel = new QLabel(tr("Description:"));
    descriptionLabel->setHidden(true);
    description = new QLabel;
    description->setHidden(true);
    description->setWordWrap(true);
    authorsLabel = new QLabel(tr("Authors:"));
    authorsLabel->setHidden(true);
    authors = new QLabel;
    authors->setHidden(true);
    authors->setWordWrap(true);
    dependsLabel = new QLabel(tr("Depends on:"));
    dependsLabel->setHidden(true);
    depends = new QLabel;
    depends->setHidden(true);
    depends->setWordWrap(true);
    usedByLabel = new QLabel(tr("Used by:"));
    usedByLabel->setHidden(true);
    usedBy = new QLabel;
    usedBy->setHidden(true);
    usedBy->setWordWrap(true);
    replacesLabel = new QLabel(tr("Replaces:"));
    replacesLabel->setHidden(true);
    replaces = new QLabel;
    replaces->setHidden(true);
    replaces->setWordWrap(true);
    replacedWithLabel = new QLabel(tr("Can be replaced with:"));
    replacedWithLabel->setHidden(true);
    replacedWith = new QLabel;
    replacedWith->setHidden(true);
    replacedWith->setWordWrap(true);

    /* Button for selecting plugin dir */
    QPushButton* pluginDirButton = new QPushButton(style()->standardIcon(QStyle::SP_DirOpenIcon), tr("Select..."));
    connect(pluginDirButton, SIGNAL(clicked(bool)), SLOT(setPluginDir()));

    /* Button for refreshing plugin dir */
    QPushButton* refreshPluginDirButton = new QPushButton(style()->standardIcon(QStyle::SP_BrowserReload), tr("Reload"));
    connect(refreshPluginDirButton, SIGNAL(clicked(bool)), SLOT(refreshPluginDir()));

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

    /* Display errorbox if something bad happened during loading/unloading plugins */
    connect(manager, SIGNAL(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
            SLOT(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));
    connect(manager, SIGNAL(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
        SLOT(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));

    /* On selection change load new row in mapper */
    connect(view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(setCurrentRow(QModelIndex)));

    /* Emit signal on edit */
    connect(pluginDir, SIGNAL(textChanged(QString)), this, SIGNAL(edited()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(edited()));

    /* Plugin dir change requires application restart */
    connect(pluginDir, SIGNAL(textChanged(QString)), this, SIGNAL(restartRequired()));

    /* Layout for plugin dir lineedit and button */
    QHBoxLayout* pluginDirLayout = new QHBoxLayout;
    pluginDirLayout->addWidget(pluginDir, 1);
    pluginDirLayout->addWidget(pluginDirButton);
    pluginDirLayout->addWidget(refreshPluginDirButton);

    /* Layout */
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(categoryDescription, 0, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Plugin directory:")), 1, 0);
    layout->addLayout(pluginDirLayout, 1, 1);
    layout->addWidget(view, 2, 0, 1, 2);
    layout->addWidget(loadStateLabel, 3, 0, Qt::AlignTop);
    layout->addWidget(loadState, 3, 1);
    layout->addWidget(descriptionLabel, 4, 0, Qt::AlignTop);
    layout->addWidget(description, 4, 1);
    layout->addWidget(authorsLabel, 5, 0, Qt::AlignTop);
    layout->addWidget(authors, 5, 1);
    layout->addWidget(dependsLabel, 6, 0, Qt::AlignTop);
    layout->addWidget(depends, 6, 1);
    layout->addWidget(usedByLabel, 7, 0, Qt::AlignTop);
    layout->addWidget(usedBy, 7, 1);
    layout->addWidget(replacesLabel, 8, 0, Qt::AlignTop);
    layout->addWidget(replaces, 8, 1);
    layout->addWidget(replacedWithLabel, 9, 0, Qt::AlignTop);
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

void PluginDialog::Tab::loadAttempt(const string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    QString message;
    switch(after) {
        case AbstractPluginManager::NotFound:
            message = tr("Plugin file cannot be found."); break;
        case AbstractPluginManager::WrongPluginVersion:
            message = tr("Wrong version number."); break;
        case AbstractPluginManager::WrongInterfaceVersion:
            message = tr("Unsupported interface version."); break;
        case AbstractPluginManager::WrongMetadataFile:
            message = tr("Error in metadata file."); break;
        case AbstractPluginManager::UnresolvedDependency:
            message = tr("Some plugin dependecy is missing."); break;
        case AbstractPluginManager::LoadFailed:
            message = tr("Error while loading plugin binary."); break;

        /* Loaded OK, don't yell at the user */
        default: return;
    }

    MessageBox::warning(this, tr("Cannot load plugin"), tr("Cannot load plugin <strong>%1</strong>:<br/><br/>%2").arg(QString::fromStdString(name)).arg(message));
}

void PluginDialog::Tab::unloadAttempt(const string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    QString message;
    switch(after) {
        case AbstractPluginManager::UnloadFailed:
            message = tr("Error while unloading plugin binary."); break;
        case AbstractPluginManager::IsRequired:
            message = tr("The plugin is required by another plugin."); break;
        case AbstractPluginManager::IsUsed:
            message = tr("The plugin is currently used."); break;

        /* Unloaded OK or is static, don't yell at the user */
        default: return;
    }

    MessageBox::warning(this, tr("Cannot unload plugin"), tr("Cannot unload plugin <strong>%1</strong>:<br /><br/>%2").arg(QString::fromStdString(name)).arg(message));
}

void PluginDialog::Tab::setCurrentRow(const QModelIndex& index) {
    mapper->setCurrentModelIndex(index);

    bool hide = false;

    hide = loadState->text().isEmpty() ? true : false;
    loadStateLabel->setHidden(hide);
    loadState->setHidden(hide);

    hide = description->text().isEmpty() ? true : false;
    descriptionLabel->setHidden(hide);
    description->setHidden(hide);

    hide = authors->text().isEmpty() ? true : false;
    authorsLabel->setHidden(hide);
    authors->setHidden(hide);

    hide = depends->text().isEmpty() ? true : false;
    dependsLabel->setHidden(hide);
    depends->setHidden(hide);

    hide = usedBy->text().isEmpty() ? true : false;
    usedBy->setHidden(hide);
    usedByLabel->setHidden(hide);

    hide = replaces->text().isEmpty() ? true : false;
    replacesLabel->setHidden(hide);
    replaces->setHidden(hide);

    hide = replacedWith->text().isEmpty() ? true : false;
    replacedWithLabel->setHidden(hide);
    replacedWith->setHidden(hide);
}

}}
