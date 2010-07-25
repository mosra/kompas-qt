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

#include "MainWindow.h"

#include <QtGui/QApplication>
#include <QtGui/QMenuBar>
#include <QtGui/QStyle>

#include "MainWindowConfigure.h"
#include "PluginManager/PluginManager.h"
#include "AbstractMapView.h"
#include "PluginDialog.h"

using namespace std;
using namespace Map2X::Core;
using namespace Map2X::PluginManager;

namespace Map2X { namespace QtGui {

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(CONFIGURATION_FILE), view(0), tileModel(0) {
    setWindowTitle("Map2X");
    statusBar();

    /* Initialize configuration file groups, if they doesn't exist */
    if(!_configuration.group("pluginDirs")) _configuration.addGroup("pluginDirs");

    /* Get values from configuration */
    string mapViewPluginDir = DATA_DIR + string("plugins/mapView/");
    string tileModelPluginDir = DATA_DIR + string("plugins/tileModel/");
    _configuration.group("pluginDirs")->value<string>("mapView", &mapViewPluginDir);
    _configuration.group("pluginDirs")->value<string>("tileModel", &tileModelPluginDir);

    /** @todo Plugin dir */
    _mapViewPluginManager = new ::PluginManager<AbstractMapView>(mapViewPluginDir);
    _tileModelPluginManager = new ::PluginManager<AbstractTileModel>(tileModelPluginDir);

    /** @todo GUI for this */
    view = _mapViewPluginManager->instance("GraphicsMapView");
    tileModel = _tileModelPluginManager->instance("OpenStreetMap");
    tileModel->setOnline(true);
    view->setTileModel(tileModel);
    view->zoomTo(10);
    view->setLayer("Mapnik");

    createActions();
    createMenus();

    setCentralWidget(view);
    resize(800, 600);
}

MainWindow::~MainWindow() {
    delete _mapViewPluginManager;
    delete _tileModelPluginManager;
}

void MainWindow::createActions() {
    /* Quit application */
    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    quitAction->setStatusTip(tr("Quit application"));
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));

    /* Move map */
    moveMapAction = new QAction(tr("Move to Prague"), this);
    zoomInAction = new QAction(tr("Zoom in"), this);
    zoomOutAction = new QAction(tr("Zoom out"), this);
    connect(moveMapAction, SIGNAL(triggered(bool)), SLOT(moveMap()));
    connect(zoomInAction, SIGNAL(triggered(bool)), view, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), view, SLOT(zoomOut()));

    /* Settings menu */
    pluginDialogAction = new QAction(tr("Plugins"), this);
    connect(pluginDialogAction, SIGNAL(triggered(bool)), SLOT(pluginDialog()));

    /* About Qt */
    aboutQtAction = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    connect(aboutQtAction, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
    /* File menu */
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(quitAction);
    fileMenu->addAction(moveMapAction);
    fileMenu->addAction(zoomInAction);
    fileMenu->addAction(zoomOutAction);

    /* Settings menu */
    QMenu* settingsMenu = menuBar()->addMenu(tr("Settings"));
    settingsMenu->addAction(pluginDialogAction);

    /* Help menu */
    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::moveMap() {
    /* Move map to some WGS coords */
    view->setCoords(Wgs84Coords(50.08333, 14.46667));
}

void MainWindow::pluginDialog() {
    PluginDialog dialog(this, this);
    dialog.exec();
}

}}
