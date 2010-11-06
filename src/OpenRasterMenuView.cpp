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

#include "OpenRasterMenuView.h"
#include "SaveRasterWizard.h"
#include "MainWindow.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

QAction* OpenRasterMenuView::createMenuAction(const std::string& pluginName) {
    AbstractRasterModel* instance = rasterManager->instance(pluginName);
    if(!instance) return 0;

    /* Skip formats which are not writeable or convertable from another */
    if(!(instance->features() & AbstractRasterModel::LoadableFromUrl)) {
        delete instance;
        return 0;
    }
    delete instance;

    QAction* action = new QAction(QString::fromStdString(rasterManager->metadata(pluginName)->name()), this);
    items.insert(action, QString::fromStdString(pluginName));

    return action;
}

void OpenRasterMenuView::trigger(QAction* action) {
    if(!items.contains(action)) return;

    MainWindow::instance()->setRasterModel(items.value(action));
}

}}
