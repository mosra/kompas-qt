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

#include "PluginDialog.h"

#include <QtGui/QTabWidget>

#include "MainWindow.h"
#include "PluginTab.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

PluginDialog::PluginDialog(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationDialog(parent, f) {
    /* Tabs */
    tabs = new QTabWidget;

    QList<PluginManagerStore::AbstractItem*> items = MainWindow::instance()->pluginManagerStore()->items();

    foreach(PluginManagerStore::AbstractItem* item, items) {
        PluginTab* tab = new PluginTab(item);
        tabs->addTab(tab, item->name());
        connectWidget(tab);
    }

    setCentralWidget(tabs);
    setWindowTitle(tr("Plugins"));
    resize(640, 400);
    setAttribute(Qt::WA_DeleteOnClose);
}

}}}
