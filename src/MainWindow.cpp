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
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QDockWidget>

#include "MainWindowConfigure.h"
#include "PluginManager.h"
#include "AbstractMapView.h"
#include "PluginDialog.h"
#include "TileDataThread.h"
#include "ConfigurationDialog.h"
#include "ToolPluginMenuView.h"
#include "SaveRasterMenuView.h"
#include "MapOptionsDock.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MainWindow* MainWindow::_instance;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(CONFIGURATION_FILE), _mapView(0), _rasterModel(0) {
    _instance = this;

    setWindowTitle("Map2X");
    statusBar();

    /* Load default configuration */
    loadDefaultConfiguration();

    _mapViewPluginManager = new PluginManager<AbstractMapView>
        (_configuration.group("plugins")->group("mapViews")->value<string>("__dir"));
    _projectionPluginManager = new PluginManager<AbstractProjection>
        (_configuration.group("plugins")->group("projections")->value<string>("__dir"));
    _rasterModelPluginManager = new PluginManager<AbstractRasterModel>
        (_configuration.group("plugins")->group("rasterModels")->value<string>("__dir"));
    _toolPluginManager = new PluginManager<AbstractTool>
        (_configuration.group("plugins")->group("tools")->value<string>("__dir"));

    /** @todo Do that in splash */
    loadPluginsAsConfigured("mapViews", _mapViewPluginManager);
    loadPluginsAsConfigured("rasterModels", _rasterModelPluginManager);
    loadPluginsAsConfigured("tools", _toolPluginManager);

    /** @todo GUI for this */
    TileDataThread::setMaxSimultaenousDownloads(_configuration.group("map")->value<int>("maxSimultaenousDownloads"));
    _mapView = _mapViewPluginManager->instance(_configuration.group("map")->value<string>("viewPlugin"));
    connect(_mapView, SIGNAL(currentCoordinates(Core::Wgs84Coords)), SLOT(currentCoordinates(Core::Wgs84Coords)));

    createActions();
    createMenus();

    setRasterModel(QString::fromStdString(_configuration.group("map")->value<string>("rasterModel")));
    _mapView->zoomTo(_configuration.group("map")->value<Zoom>("zoom"));
    _mapView->setCoords(_configuration.group("map")->value<Wgs84Coords>("homePosition"));
    _mapView->setLayer(QString::fromStdString(_configuration.group("map")->value<string>("rasterLayer")));

    QDockWidget* mapOptionsDock = new QDockWidget;
    mapOptionsDock->setWidget(new MapOptionsDock(this, this));
    mapOptionsDock->setWindowTitle(tr("Map options"));
    addDockWidget(Qt::RightDockWidgetArea, mapOptionsDock);

    /* Save raster map menu */
    SaveRasterMenuView* saveRasterMenuView = new SaveRasterMenuView(_rasterModelPluginManager, saveRasterMenu, 0, this);
    saveRasterMenuView->update();

    /* Tools menu */
    ToolPluginMenuView* menuView = new ToolPluginMenuView(this, _toolPluginManager, toolsMenu, 0, this);
    menuView->update();

    /* Status bar with coordinates */
    coordinateStatus = new QLabel;
    statusBar()->addPermanentWidget(coordinateStatus);

    setCentralWidget(_mapView);
    resize(800, 600);
}

MainWindow::~MainWindow() {
    delete _mapViewPluginManager;
    delete _rasterModelPluginManager;
}

void MainWindow::loadDefaultConfiguration() {
    _configuration.setAutomaticGroupCreation(true);
    _configuration.setAutomaticKeyCreation(true);

    /* Plugin dirs */
    string mapViewPluginDir = PLUGIN_MAPVIEW_DIR;
    string projectionPluginDir = PLUGIN_PROJECTION_DIR;
    string rasterModelPluginDir = PLUGIN_RASTERMODEL_DIR;
    string toolPluginDir = PLUGIN_TOOL_DIR;
    _configuration.group("plugins")->group("mapViews")->value<string>("__dir", &mapViewPluginDir);
    _configuration.group("plugins")->group("projections")->value<string>("__dir", &projectionPluginDir);
    _configuration.group("plugins")->group("rasterModels")->value<string>("__dir", &rasterModelPluginDir);
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
    string rasterModel = "OpenStreetMapRasterModel";
    string rasterLayer = "Mapnik";
    Zoom zoom        = 4;
    _configuration.group("map")->value("rasterModel", &rasterModel);
    _configuration.group("map")->value("rasterLayer", &rasterLayer);
    _configuration.group("map")->value("zoom", &zoom);

    _configuration.setAutomaticGroupCreation(false);
    _configuration.setAutomaticKeyCreation(false);
}

void MainWindow::setRasterModel(const QString& name) {
    lockRasterModelForWrite();

    _rasterModel = _rasterModelPluginManager->instance(name.toStdString());

    /** @todo Disable Save Raster menu when no writeable format is available at all */
    /** @todo If current raster model doesn't have NonCOnvertableFormat, it's shown in the menu twice. */

    /* Raster model is available, configure it & enable save menu */
    if(_rasterModel) {
        saveRasterMenu->setDisabled(false);
        _rasterModel->setOnline(_configuration.group("map")->value<bool>("online"));

        /* Update action in "save raster" menu */
        saveRasterAction->setText(tr("Offline %0 package").arg(
            QString::fromStdString(_rasterModelPluginManager->metadata(name.toStdString())->name())
        ));
        if(_rasterModel->features() & AbstractRasterModel::WriteableFormat)
            saveRasterAction->setDisabled(false);
        else
            saveRasterAction->setDisabled(true);

    /* Raster model is not available, disable save menu */
    } else saveRasterMenu->setDisabled(true);

    unlockRasterModel();

    _mapView->updateRasterModel();
}

void MainWindow::createActions() {
    /* Save raster map */
    saveRasterAction = new QAction(this);

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

    /* Save raster map menu */
    saveRasterMenu = fileMenu->addMenu(tr("Save map as..."));
    saveRasterMenu->addAction(saveRasterAction);
    saveRasterMenu->addSeparator();
    saveRasterMenu->setDisabled(true);

    fileMenu->addSeparator();
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
    PluginDialog dialog(this);
    dialog.exec();
}

void MainWindow::configurationDialog() {
    ConfigurationDialog dialog(this);
    dialog.exec();
}

void MainWindow::currentCoordinates(const Map2X::Core::Wgs84Coords& coords) {
    coordinateStatus->setText(QString::fromStdString(coords.toString(1)));
}

}}
