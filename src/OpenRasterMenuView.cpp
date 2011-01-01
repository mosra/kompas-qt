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

#include "OpenRasterMenuView.h"
#include "SaveRasterWizard.h"
#include "MainWindow.h"

using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void OpenRasterMenuView::clearMenu() {
    qDeleteAll<QList<QAction*> >(items.keys());
    items.clear();
}

QAction* OpenRasterMenuView::createMenuAction(const std::string& pluginName) {
    AbstractRasterModel* instance = rasterManager->instance(pluginName);
    if(!instance) return 0;

    /* Skip formats which are not writeable or convertable from another */
    if(!(instance->features() & AbstractRasterModel::LoadableFromUrl)) {
        delete instance;
        return 0;
    }

    QAction* action = new QAction(QString::fromStdString(*instance->metadata()->name()), this);
    items.insert(action, QString::fromStdString(pluginName));

    delete instance;

    return action;
}

void OpenRasterMenuView::trigger(QAction* action) {
    if(!items.contains(action)) return;

    AbstractRasterModel* model = MainWindow::instance()->rasterModelPluginManager()->instance(items.value(action).toStdString());
    model->setOnline(true);

    MainWindow::instance()->setRasterModel(model);
}

}}
