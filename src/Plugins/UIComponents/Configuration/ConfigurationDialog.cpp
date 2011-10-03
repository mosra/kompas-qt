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

#include "ConfigurationDialog.h"

#include <QtGui/QTabWidget>

#include "MainTab.h"
#include "CacheTab.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

ConfigurationDialog::ConfigurationDialog(QWidget* parent, Qt::WindowFlags f): AbstractConfigurationDialog(parent, f) {
    /* Tabs */
    QTabWidget* tabs = new QTabWidget;

    MainTab* main = new MainTab;
    tabs->addTab(main, tr("Main configuration"));
    connectWidget(main);

    CacheTab* cache = new CacheTab;
    tabs->addTab(cache, tr("Cache"));
    connectWidget(cache);

    setCentralWidget(tabs);
    setWindowTitle(tr("Kompas configuration"));
    resize(480, 280);
}

}}}
