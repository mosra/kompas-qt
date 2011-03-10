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

#include "SaveRasterMenuView.h"
#include "SaveRasterWizard.h"
#include "MainWindow.h"

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

void SaveRasterMenuView::clearMenu() {
    qDeleteAll<QList<QAction*> >(actions.keys());
    actions.clear();

    /* Get current raster map plugin name */
    Locker<const AbstractRasterModel> model = MainWindow::instance()->rasterModelForRead();
    if(model()) currentModel = model()->plugin();
    else        currentModel.clear();
}

QAction* SaveRasterMenuView::createMenuAction(const std::string& pluginName) {
    /* Don't show current model in menu */
    if(pluginName == currentModel) return 0;

    AbstractRasterModel* instance = rasterManager->instance(pluginName);
    if(!instance) return 0;

    /* Skip formats which are not writeable or convertable from another */
    if(!(instance->features() & AbstractRasterModel::WriteableFormat) || (instance->features() & AbstractRasterModel::NonConvertableFormat)) {
        delete instance;
        return 0;
    }

    QAction* a = new QAction(QString::fromStdString(*instance->metadata()->name()), this);
    actions.insert(a, pluginName);

    delete instance;

    return a;
}

void SaveRasterMenuView::trigger(QAction* action) {
    if(!actions.contains(action)) return;

    SaveRasterWizard wizard(actions.value(action));
    wizard.exec();
}

}}}
