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
#include <QtGui/QFileDialog>

#include "MainWindowConfigure.h"
#include "PluginManager.h"
#include "PluginDialog.h"
#include "TileDataThread.h"
#include "ConfigurationDialog.h"
#include "ToolPluginMenuView.h"
#include "SaveRasterMenuView.h"
#include "OpenRasterMenuView.h"
#include "MapOptionsDock.h"
#include "RasterPackageModel.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "RasterZoomModel.h"
#include "SaveRasterWizard.h"
#include "MessageBox.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

MainWindow* MainWindow::_instance;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(CONFIGURATION_FILE), _mapView(0), _rasterModel(0) {
    _instance = this;

    /* Window icon */
    QIcon icon;
    icon.addFile(":/logo-16.png");
    icon.addFile(":/logo-32.png");
    icon.addFile(":/logo-64.png");
    icon.addFile(":/logo-128.png");
    icon.addFile(":/logo-256.png");

    setWindowTitle("Map2X");
    setWindowIcon(icon);
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

    _rasterPackageModel = new RasterPackageModel(this);
    _rasterLayerModel = new RasterLayerModel(this);
    _rasterOverlayModel = new RasterOverlayModel(this);
    _rasterZoomModel = new RasterZoomModel(this);

    /** @todo Do that in splash */
    loadPluginsAsConfigured("mapViews", _mapViewPluginManager);
    loadPluginsAsConfigured("rasterModels", _rasterModelPluginManager);
    loadPluginsAsConfigured("tools", _toolPluginManager);

    TileDataThread::setMaxSimultaenousDownloads(_configuration.group("map")->value<int>("maxSimultaenousDownloads"));

    /* Map view plugin */
    setMapView(_mapViewPluginManager->instance(_configuration.group("map")->value<string>("viewPlugin")));

    createActions();
    createMenus();

    QDockWidget* mapOptionsDock = new QDockWidget;
    mapOptionsDock->setWidget(new MapOptionsDock(this));
    mapOptionsDock->setWindowTitle(tr("Map options"));
    addDockWidget(Qt::RightDockWidgetArea, mapOptionsDock);

    /* Open raster map menu */
    OpenRasterMenuView* openRasterMenuView = new OpenRasterMenuView(_rasterModelPluginManager, openRasterMenu, 0, this);
    openRasterMenuView->update();

    /* Save raster map menu */
    saveRasterMenuView = new SaveRasterMenuView(_rasterModelPluginManager, saveRasterMenu, 0, this);
    saveRasterMenuView->update();

    /* Tools menu */
    ToolPluginMenuView* menuView = new ToolPluginMenuView(this, _toolPluginManager, toolsMenu, 0, this);
    menuView->update();

    /* Status bar with coordinates */
    coordinateStatus = new QLabel;
    statusBar()->addPermanentWidget(coordinateStatus);

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

    /* Maximal count of simultaenous downloads */
    unsigned int maxSimultaenousDownloads = 3;
    _configuration.group("map")->value("maxSimultaenousDownloads", &maxSimultaenousDownloads);

    _configuration.setAutomaticGroupCreation(false);
    _configuration.setAutomaticKeyCreation(false);
}

void MainWindow::setMapView(AbstractMapView* view) {
    /** @todo Allow no map view */
    if(!view) return;
    if(_mapView) delete _mapView;

    _mapView = view;
    connect(_mapView, SIGNAL(currentCoordinates(Core::Wgs84Coords)), SLOT(currentCoordinates(Core::Wgs84Coords)));
    setCentralWidget(_mapView);
}

void MainWindow::setRasterModel(AbstractRasterModel* model) {
    /** @todo Disable Save Raster menu when no writeable format is available at all */

    /* Raster model is available, configure it & enable save menu */
    if(model) {
        saveRasterMenu->setDisabled(false);
        closeRasterAction->setDisabled(false);

        /* Update action in "save raster" menu */
        saveRasterAction->setText(tr("Offline %0 package").arg(
            QString::fromStdString(_rasterModelPluginManager->metadata(model->name())->name())
        ));
        if(model->features() & AbstractRasterModel::WriteableFormat)
            saveRasterAction->setDisabled(false);
        else
            saveRasterAction->setDisabled(true);

    /* Raster model is not available, disable save menu */
    } else {
        saveRasterMenu->setDisabled(true);
        closeRasterAction->setDisabled(true);
    }

    lockRasterModelForWrite();
    delete _rasterModel;
    _rasterModel = model;
    unlockRasterModel();

    /* Update save raster menu to avoid showing the same plugin twice */
    saveRasterMenuView->update();

    _rasterPackageModel->reload();
    _rasterLayerModel->reload();
    _rasterOverlayModel->reload();
    _rasterZoomModel->reload();

    emit rasterModelChanged();
}

void MainWindow::setOnlineEnabled(bool enabled) {
    lockRasterModelForWrite();
    _rasterModel->setOnline(enabled);
    unlockRasterModel();

    _rasterPackageModel->reload();
    _rasterLayerModel->reload();
    _rasterOverlayModel->reload();
    _rasterZoomModel->reload();

    emit rasterModelChanged();
}

void MainWindow::openRaster() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select map file"));
    if(filename.isEmpty()) return;

    /* Try to open the package with current model */
    if(_rasterModel && _rasterModel->addPackage(filename.toStdString()) != -1) {
        _rasterPackageModel->reload();
        _rasterLayerModel->reload();
        _rasterOverlayModel->reload();
        _rasterZoomModel->reload();

        emit rasterModelChanged();

    /* Else try all plugins */
    } else {
        std::ifstream i(filename.toUtf8().constData());
        AbstractRasterModel* firstSupport = 0;

        vector<string> plugins = _rasterModelPluginManager->nameList();
        for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
            /* Skip not loaded plugins */
            if(!(_rasterModelPluginManager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
                continue;

            /* Instance of the model */
            AbstractRasterModel* instance = _rasterModelPluginManager->instance(*it);
            if(!instance) continue;

            /* Skip models which cannot recognize its file format */
            if(!(instance->features() & AbstractRasterModel::SelfRecognizable)) {
                delete instance;
                continue;
            }

            /* Rewind file and try to recognize it */
            i.clear();
            i.seekg(0, ios::beg);
            int state = instance->recognizeFile(filename.toStdString(), i);

            /* Try to get full supporting plugin, if not found, go with first
                partially supporting. */
            if(state == AbstractRasterModel::PartiallySupported && !firstSupport)
                firstSupport = instance;
            else if(state == AbstractRasterModel::FullySupported) {
                delete firstSupport;
                firstSupport = instance;
                break;

            /* Otherwise delete instance and continue with another */
            } else delete instance;
        }

        i.close();

        /* No supporting plugin found */
        if(!firstSupport) {
            MessageBox::warning(this, tr("Unsupported file format"), tr("No suitable map plugin was found for this file."));
            return;
        }

        /* If package cannot be opened, destroy that bitch and go home */
        if(firstSupport->addPackage(filename.toStdString())) {
            MessageBox::warning(this, tr("Cannot open file"), tr("The package cannot be loaded."));
            delete firstSupport;
            return;
        }

        /* Replace current raster model with new */
        setRasterModel(firstSupport);
    }
}

void MainWindow::saveRaster() {
    string name;

    lockRasterModelForRead();
    if(_rasterModel) name = _rasterModel->name();
    unlockRasterModel();

    if(name.empty()) return;

    SaveRasterWizard wizard(name);
    wizard.exec();
}

void MainWindow::createActions() {
    /* Open raster map */
    openRasterAction = new QAction(tr("Open map package"), this);
    connect(openRasterAction, SIGNAL(triggered(bool)), SLOT(openRaster()));

    /* Save raster map */
    saveRasterAction = new QAction(this);
    connect(saveRasterAction, SIGNAL(triggered(bool)), SLOT(saveRaster()));

    /* Close raster map */
    closeRasterAction = new QAction(tr("Close map"), this);
    closeRasterAction->setDisabled(true);
    connect(closeRasterAction, SIGNAL(triggered(bool)), SLOT(closeRaster()));

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
    fileMenu->addAction(openRasterAction);

    /* Open raster map menu */
    openRasterMenu = fileMenu->addMenu(tr("Load online map"));

    /* Save raster map menu */
    saveRasterMenu = fileMenu->addMenu(tr("Save map"));
    saveRasterMenu->addAction(saveRasterAction);
    saveRasterMenu->addSeparator();
    saveRasterMenu->setDisabled(true);

    fileMenu->addAction(closeRasterAction);
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
