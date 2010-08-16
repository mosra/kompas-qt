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
#include "TileDataThread.h"
#include "ConfigurationDialog.h"
#include "ToolPluginMenuView.h"
#include "MapOptionsDock.h"

using namespace std;
using namespace Map2X::Core;
using namespace Map2X::PluginManager;

namespace Map2X { namespace QtGui {

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(CONFIGURATION_FILE), _mapView(0), _tileModel(0) {
    setWindowTitle("Map2X");
    statusBar();

    /* Load default configuration */
    loadDefaultConfiguration();

    _mapViewPluginManager = new ::PluginManager<AbstractMapView>
        (_configuration.group("pluginDirs")->value<string>("mapView"));
    _tileModelPluginManager = new ::PluginManager<AbstractTileModel>
        (_configuration.group("pluginDirs")->value<string>("tileModel"));
    _toolPluginManager = new ::PluginManager<AbstractTool>
        (_configuration.group("pluginDirs")->value<string>("tools"));

    /** @todo GUI for this */
    _mapView = _mapViewPluginManager->instance(_configuration.group("map")->value<string>("viewPlugin"));
    _tileModel = _tileModelPluginManager->instance(_configuration.group("map")->value<string>("tileModel"));
    _tileModel->setOnline(_configuration.group("map")->value<bool>("online"));
    TileDataThread::setMaxSimultaenousDownloads(_configuration.group("map")->value<int>("maxSimultaenousDownloads"));
    _mapView->setTileModel(_tileModel);
    _mapView->zoomTo(_configuration.group("map")->value<Zoom>("zoom"));
    _mapView->setCoords(_configuration.group("map")->value<Wgs84Coords>("homePosition"));
    _mapView->setLayer(QString::fromStdString(_configuration.group("map")->value<string>("tileLayer")));

    QDockWidget* mapOptionsDock = new QDockWidget;
    mapOptionsDock->setWidget(new MapOptionsDock(this, this));
    mapOptionsDock->setWindowTitle(tr("Map options"));
    addDockWidget(Qt::RightDockWidgetArea, mapOptionsDock);

    createActions();
    createMenus();

    /* Tools menu */
    ToolPluginMenuView* menuView = new ToolPluginMenuView(this, _toolPluginManager, toolsMenu, 0, this);

    setCentralWidget(_mapView);
    resize(800, 600);
}

MainWindow::~MainWindow() {
    delete _mapViewPluginManager;
    delete _tileModelPluginManager;
}

void MainWindow::loadDefaultConfiguration() {
    _configuration.setAutomaticGroupCreation(true);
    _configuration.setAutomaticKeyCreation(true);

    /* Plugin dirs */
    string mapViewPluginDir = DATA_DIR + string("plugins/mapView/");
    string tileModelPluginDir = DATA_DIR + string("plugins/tileModel/");
    string toolPluginDir = DATA_DIR + string("plugins/tools/");
    _configuration.group("pluginDirs")->value<string>("mapView", &mapViewPluginDir);
    _configuration.group("pluginDirs")->value<string>("tileModel", &tileModelPluginDir);
    _configuration.group("pluginDirs")->value<string>("tools", &toolPluginDir);

    /* Plugin for map view */
    string mapViewPlugin = "GraphicsMapView";
    _configuration.group("map")->value("viewPlugin", &mapViewPlugin);

    /* Enabled online maps? */
    bool onlineEnabled = true;
    _configuration.group("map")->value("online", &onlineEnabled);

    /* Maximal count of simultaenous downloads */
    unsigned int maxSimultaenousDownloads = 3;
    _configuration.group("map")->value("maxSimultaenousDownloads", &maxSimultaenousDownloads);

    /* Home position */
    Wgs84Coords homePosition(50.088, 14.354);
    _configuration.group("map")->value("homePosition", &homePosition);

    /* Default tile model, layer, overlays and zoom */
    string tileModel = "OpenStreetMap";
    string tileLayer = "Mapnik";
    Zoom zoom        = 4;
    _configuration.group("map")->value("tileModel", &tileModel);
    _configuration.group("map")->value("tileLayer", &tileLayer);
    _configuration.group("map")->value("zoom", &zoom);

    _configuration.setAutomaticGroupCreation(false);
    _configuration.setAutomaticKeyCreation(false);
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
    connect(zoomInAction, SIGNAL(triggered(bool)), _mapView, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), _mapView, SLOT(zoomOut()));

    /* Settings menu */
    pluginDialogAction = new QAction(tr("Plugins"), this);
    configurationDialogAction = new QAction(tr("Configure Map2X"), this);
    connect(pluginDialogAction, SIGNAL(triggered(bool)), SLOT(pluginDialog()));
    connect(configurationDialogAction, SIGNAL(triggered(bool)), SLOT(configurationDialog()));

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

    /* Tools menu */
    toolsMenu = menuBar()->addMenu(tr("Tools"));

    /* Settings menu */
    QMenu* settingsMenu = menuBar()->addMenu(tr("Settings"));
    settingsMenu->addAction(pluginDialogAction);
    settingsMenu->addAction(configurationDialogAction);

    /* Help menu */
    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::moveMap() {
    /* Move map to some WGS coords */
    _mapView->setCoords(Wgs84Coords(50.08333, 14.46667));
}

void MainWindow::pluginDialog() {
    PluginDialog dialog(this, this);
    dialog.exec();
}

void MainWindow::configurationDialog() {
    ConfigurationDialog dialog(this, this);
    dialog.exec();
}

}}
