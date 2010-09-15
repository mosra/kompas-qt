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
#include "PluginManager.h"
#include "AbstractMapView.h"
#include "PluginDialog.h"
#include "TileDataThread.h"
#include "ConfigurationDialog.h"
#include "ToolPluginMenuView.h"
#include "MapOptionsDock.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(CONFIGURATION_FILE), _mapView(0), _tileModel(0) {
    setWindowTitle("Map2X");
    statusBar();

    /* Load default configuration */
    loadDefaultConfiguration();

    _mapViewPluginManager = new PluginManager<AbstractMapView>
        (_configuration.group("plugins")->group("mapViews")->value<string>("__dir"));
    _tileModelPluginManager = new PluginManager<AbstractTileModel>
        (_configuration.group("plugins")->group("tileModels")->value<string>("__dir"));
    _toolPluginManager = new PluginManager<AbstractTool>
        (_configuration.group("plugins")->group("tools")->value<string>("__dir"));

    /** @todo Do that in splash */
    loadPluginsAsConfigured("mapViews", _mapViewPluginManager);
    loadPluginsAsConfigured("tileModels", _tileModelPluginManager);
    loadPluginsAsConfigured("tools", _toolPluginManager);

    /** @todo GUI for this */
    TileDataThread::setMaxSimultaenousDownloads(_configuration.group("map")->value<int>("maxSimultaenousDownloads"));
    _mapView = _mapViewPluginManager->instance(_configuration.group("map")->value<string>("viewPlugin"));
    setTileModel(QString::fromStdString(_configuration.group("map")->value<string>("tileModel")));
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
    string mapViewPluginDir = DATA_DIR + string("plugins/mapViews/");
    string tileModelPluginDir = DATA_DIR + string("plugins/tileModels/");
    string toolPluginDir = DATA_DIR + string("plugins/tools/");
    _configuration.group("plugins")->group("mapViews")->value<string>("__dir", &mapViewPluginDir);
    _configuration.group("plugins")->group("tileModels")->value<string>("__dir", &tileModelPluginDir);
    _configuration.group("plugins")->group("tools")->value<string>("__dir", &toolPluginDir);

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

void MainWindow::setTileModel(const QString& name) {
    _tileModel = _tileModelPluginManager->instance(name.toStdString());

    if(_tileModel) {
        _tileModel->setOnline(_configuration.group("map")->value<bool>("online"));
        _mapView->setTileModel(_tileModel);

    } else _mapView->setTileModel(0);
}

void MainWindow::createActions() {
    /* Quit application */
    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));

    /* Move map */
    /** @todo Disable when zooming in/out is not possible */
    zoomInAction = new QAction(tr("Zoom in"), this);
    zoomInAction->setShortcut(Qt::CTRL|Qt::Key_Plus);
    zoomOutAction = new QAction(tr("Zoom out"), this);
    zoomOutAction->setShortcut(Qt::CTRL|Qt::Key_Minus);
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

    /* Map menu */
    QMenu* mapMenu = menuBar()->addMenu(tr("Map"));
    mapMenu->addAction(zoomInAction);
    mapMenu->addAction(zoomOutAction);

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

void MainWindow::loadPluginsAsConfigured(const std::string& group, AbstractPluginManager* manager) {
    vector<string> names = manager->nameList();

    for(vector<string>::const_iterator it = names.begin(); it != names.end(); ++it)
        if(configuration()->group("plugins")->group(group)->value<bool>(*it))
            manager->load(*it);
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
