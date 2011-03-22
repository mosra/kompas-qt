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

#include "OpenRasterUIComponent.h"

#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>

#include "MainWindow.h"
#include "PluginManager.h"
#include "MessageBox.h"
#include "AbstractRasterModel.h"
#include "PluginManagerStore.h"
#include "OpenRasterMenuView.h"

PLUGIN_REGISTER_STATIC(OpenRasterUIComponent,
                       Kompas::Plugins::UIComponents::OpenRasterUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

OpenRasterUIComponent::OpenRasterUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* Open raster map */
    QIcon openPackageIcon;
    openPackageIcon.addFile(":/open-package-16.png");
    openPackageIcon.addFile(":/open-package-64.png");
    QAction* openRasterAction = new QAction(openPackageIcon, tr("Open map package"), this);
    openRasterAction->setData("openRaster");
    connect(openRasterAction, SIGNAL(triggered(bool)), SLOT(openRaster()));

    /* Open online map */
    QIcon openOnlineIcon;
    openOnlineIcon.addFile(":/open-online-16.png");
    openOnlineIcon.addFile(":/open-online-64.png");
    QAction* openOnlineAction = new QAction(openOnlineIcon, tr("Load online map"), this);
    openOnlineAction->setData("openOnline");

    /* Close raster map */
    closeRasterAction = new QAction(QIcon(":/close-16.png"), tr("Close map"), this);
    closeRasterAction->setDisabled(true);
    connect(closeRasterAction, SIGNAL(triggered(bool)), SLOT(closeRaster()));

    _actions << openRasterAction << openOnlineAction << closeRasterAction;

    /* Open raster map menu */
    QMenu* openRasterMenu = new QMenu(MainWindow::instance());
    openOnlineAction->setMenu(openRasterMenu);
    OpenRasterMenuView* openRasterMenuView = new OpenRasterMenuView(MainWindow::instance()->pluginManagerStore()->rasterModels()->manager(), openRasterMenu, 0, this);
    openRasterMenuView->update();

    /* Disable "close raster" when model is unusable */
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)),
            this, SLOT(rasterModelChanged()));
}

void OpenRasterUIComponent::openRaster() {
    QtGui::PluginManager<AbstractRasterModel>* rasterModelPluginManager = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager();

    /* Compose filter from all loaded plugins */
    QString filter;
    vector<string> plugins = rasterModelPluginManager->pluginList();
    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(rasterModelPluginManager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
            continue;

        /* Instance of the model */
        AbstractRasterModel* instance = rasterModelPluginManager->instance(*it);
        if(!instance) continue;

        vector<string> extensions = instance->fileExtensions();
        if(extensions.empty()) {
            delete instance;
            continue;
        }

        filter += QString::fromStdString(*instance->metadata()->name()) + " (";

        for(vector<string>::const_iterator it = extensions.begin(); it != extensions.end(); ++it) {
            filter += QString::fromStdString(*it) + " ";
        }

        filter = filter.left(filter.length()-1) + ");;";

        delete instance;
    }

    filter += tr("All files (*)");

    QString filename = QFileDialog::getOpenFileName(MainWindow::instance(), tr("Select map file"), QString::fromStdString(MainWindow::instance()->configuration()->group("paths")->value<string>("packages")), filter);
    if(filename.isEmpty()) return;

    AbstractRasterModel::SupportLevel supportLevel;
    AbstractRasterModel* rasterModel = MainWindow::instance()->rasterModelForFile(filename, &supportLevel);

    if(!rasterModel) {
        /* Not supported format */
        if(supportLevel == AbstractRasterModel::NotSupported)
            MessageBox::warning(MainWindow::instance(), tr("Unsupported file format"), tr("No suitable map plugin was found for this file."));

        /* Supported format, opened by current model */
        else return;
    }

    /* If package cannot be opened or is unusable, destroy that bitch and
        go home */
    if(rasterModel->addPackage(filename.toStdString()) == -1 || !rasterModel->isUsable()) {
        MessageBox::warning(MainWindow::instance(), tr("Cannot open file"), tr("The package cannot be loaded."));
        delete rasterModel;
        return;
    }

    /* Replace current raster model with new */
    MainWindow::instance()->setRasterModel(rasterModel);

    /* Package format is deprecated */
    if(supportLevel == AbstractRasterModel::DeprecatedSupport) {
        MessageBox::warning(MainWindow::instance(), tr("Deprecated file format"), tr("The package has deprecated format that probably won't be supported in future versions. Please save the package to a newer format."));
    }
}

void OpenRasterUIComponent::closeRaster() {
    MainWindow::instance()->setRasterModel(0);
}

void OpenRasterUIComponent::rasterModelChanged() {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    rasterModel.unlock();

    /* Enable "close raster" only if map view and raster is available */
    closeRasterAction->setEnabled(MainWindow::instance()->mapView() && isUsable);
}

}}}
