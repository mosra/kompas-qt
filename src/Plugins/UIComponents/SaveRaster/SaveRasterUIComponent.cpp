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

#include "SaveRasterUIComponent.h"

#include <QtGui/QAction>

#include "MainWindow.h"
#include "SaveRasterMenuView.h"
#include "SaveRasterWizard.h"
#include "PluginManagerStore.h"

PLUGIN_REGISTER_STATIC(SaveRasterUIComponent,
                       Kompas::Plugins::UIComponents::SaveRasterUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SaveRasterUIComponent::SaveRasterUIComponent(PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* Save raster map */
    QAction* saveRasterAction = new QAction(QIcon(":/save-16.png"), tr("Save map"), this);
    _actions << saveRasterAction;

    /* Save raster map to current model */
    _saveCurrentRasterAction = new QAction(this);
    connect(_saveCurrentRasterAction, SIGNAL(triggered(bool)), SLOT(saveRaster()));

    /* Save raster map menu */
    _saveRasterMenu = new QMenu(MainWindow::instance());
    saveRasterAction->setMenu(_saveRasterMenu);
    _saveRasterMenu->addAction(_saveCurrentRasterAction);
    _saveRasterMenu->addSeparator();
    _saveRasterMenu->setDisabled(true);
    _saveRasterMenuView = new SaveRasterMenuView(MainWindow::instance()->pluginManagerStore()->rasterModels()->manager(), _saveRasterMenu, 0, this);
    _saveRasterMenuView->update();

    /* Update save raster menu to avoid showing the same plugin twice */
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)), this, SLOT(rasterModelChanged(const Core::AbstractRasterModel*)));
}

void SaveRasterUIComponent::saveRaster() {
    string plugin;

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    if(rasterModel()) plugin = rasterModel()->plugin();
    rasterModel.unlock();

    if(plugin.empty()) return;

    SaveRasterWizard wizard(plugin);
    wizard.exec();
}

void SaveRasterUIComponent::rasterModelChanged(const AbstractRasterModel* previous) {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    QString name = rasterModel() ? QString::fromStdString(*rasterModel()->metadata()->name()) : "";
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    bool isWriteable = rasterModel() ? (rasterModel()->features() & AbstractRasterModel::WriteableFormat) : false;
    rasterModel.unlock();

    _saveRasterMenuView->update();

    if(MainWindow::instance()->mapView() && isUsable) {
        _saveRasterMenu->setDisabled(false);

        /* Update action in "save raster" menu */
        _saveCurrentRasterAction->setText(tr("Offline %0 package").arg(name));
        if(isWriteable)
            _saveCurrentRasterAction->setDisabled(false);
        else
            _saveCurrentRasterAction->setDisabled(true);
    } else
        _saveRasterMenu->setDisabled(true);
}

}}}
