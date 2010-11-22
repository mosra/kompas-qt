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

#include "SaveRasterMenuView.h"
#include "SaveRasterWizard.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

void SaveRasterMenuView::clearMenu() {
    qDeleteAll<QList<QAction*> >(actions.keys());
    actions.clear();
}

QAction* SaveRasterMenuView::createMenuAction(const std::string& pluginName) {
    AbstractRasterModel* instance = rasterManager->instance(pluginName);
    if(!instance) return 0;

    /* Skip formats which are not writeable or convertable from another */
    if(!(instance->features() & AbstractRasterModel::WriteableFormat) || (instance->features() & AbstractRasterModel::NonConvertableFormat)) {
        delete instance;
        return 0;
    }
    delete instance;

    QAction* a = new QAction(QString::fromStdString(rasterManager->metadata(pluginName)->name()), this);
    actions.insert(a, pluginName);
    return a;
}

void SaveRasterMenuView::trigger(QAction* action) {
    if(!actions.contains(action)) return;

    SaveRasterWizard wizard(actions.value(action));
    wizard.exec();
}

}}
