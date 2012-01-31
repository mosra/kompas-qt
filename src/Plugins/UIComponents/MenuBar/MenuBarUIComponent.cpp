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

#include "MenuBarUIComponent.h"

#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtCore/QDebug>

#include "MainWindow.h"

using namespace Kompas::QtGui;

PLUGIN_REGISTER_STATIC(MenuBarUIComponent,
                       Kompas::Plugins::UIComponents::MenuBarUIComponent,
                       "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

namespace Kompas { namespace Plugins { namespace UIComponents {

MenuBarUIComponent::MenuBarUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    _menuBar = new QMenuBar(MainWindow::instance());
    _fileMenu = _menuBar->addMenu(QObject::tr("File"));
    _toolsMenu = _menuBar->addMenu(QObject::tr("Tools"));
    _settingsMenu = _menuBar->addMenu(QObject::tr("Settings"));
    _helpMenu = _menuBar->addMenu(QObject::tr("Help"));

    _packagesSeparator = _fileMenu->addSeparator();

    _quitSeparator = _fileMenu->addSeparator();
    _quitAction = new QAction(QIcon(":/exit-16.png"), QObject::tr("Quit"), this);
    _quitAction->setShortcut(QKeySequence::Quit);
    connect(_quitAction, SIGNAL(triggered(bool)), MainWindow::instance(), SLOT(close()));
    _fileMenu->addAction(_quitAction);
}

void MenuBarUIComponent::actionAdded(int category, QAction* action) {
    if(category == AbstractUIComponent::Sessions)
        _fileMenu->insertAction(_packagesSeparator, action);
    else if(category == AbstractUIComponent::Maps)
        _fileMenu->insertAction(_quitSeparator, action);
    else if(category == AbstractUIComponent::Tools)
        _toolsMenu->addAction(action);
    else if(category == AbstractUIComponent::Settings)
        _settingsMenu->addAction(action);
    else if(category == AbstractUIComponent::Help)
        _helpMenu->addAction(action);
}

}}}
