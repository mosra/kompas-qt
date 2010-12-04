/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
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
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QStackedWidget>

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

#define WELCOME_SCREEN 0
#define MAP_VIEW 1

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

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

    setWindowTitle("Kompas");
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

    createActions();
    createMenus();

    /* Welcome screen, wrapped in another widget so it's nicely centered */
    QFrame* welcomeScreenFrame = new QFrame;
    welcomeScreenFrame->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/welcome-640.png")));
    welcomeScreenFrame->setPalette(palette);

    QToolButton* openSessionButton = new QToolButton(this);
    openSessionButton->setDefaultAction(openSessionAction);
    openSessionButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openSessionButton->setAutoRaise(true);
    openSessionButton->setIconSize(QSize(64, 64));
    openSessionButton->setFixedHeight(96);

    QToolButton* openRasterButton = new QToolButton(this);
    openRasterButton->setDefaultAction(openRasterAction);
    openRasterButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openRasterButton->setAutoRaise(true);
    openRasterButton->setIconSize(QSize(64, 64));
    openRasterButton->setFixedHeight(96);

    QToolButton* openOnlineButton = new QToolButton(this);
    openOnlineButton->setDefaultAction(openOnlineAction);
    openOnlineButton->setPopupMode(QToolButton::InstantPopup);
    openOnlineButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openOnlineButton->setAutoRaise(true);
    openOnlineButton->setIconSize(QSize(64, 64));
    openOnlineButton->setFixedHeight(96);

    QGridLayout* welcomeScreenFrameLayout = new QGridLayout;
    welcomeScreenFrameLayout->addWidget(new QWidget, 0, 0, 1, 5);
    welcomeScreenFrameLayout->addWidget(new QWidget, 1, 0);
    welcomeScreenFrameLayout->addWidget(openSessionButton, 1, 1);
    welcomeScreenFrameLayout->addWidget(openRasterButton, 1, 2);
    welcomeScreenFrameLayout->addWidget(openOnlineButton, 1, 3);
    welcomeScreenFrameLayout->addWidget(new QWidget, 1, 4);
    welcomeScreenFrameLayout->addWidget(new QWidget, 2, 0, 1, 5);
    welcomeScreenFrameLayout->setColumnMinimumWidth(0, 100);
    welcomeScreenFrameLayout->setColumnMinimumWidth(4, 100);
    welcomeScreenFrameLayout->setRowMinimumHeight(0, 310);
    welcomeScreenFrameLayout->setRowMinimumHeight(1, 120);
    welcomeScreenFrameLayout->setRowMinimumHeight(2, 50);

    welcomeScreenFrame->setLayout(welcomeScreenFrameLayout);
    welcomeScreenFrame->setFixedSize(640, 480);
    QWidget* welcomeScreen = new QWidget;
    QHBoxLayout* welcomeScreenLayout = new QHBoxLayout;
    welcomeScreenLayout->addWidget(welcomeScreenFrame);
    welcomeScreen->setLayout(welcomeScreenLayout);

    /* Stacked widget */
    centralStackedWidget = new QStackedWidget;
    centralStackedWidget->addWidget(welcomeScreen);
    setCentralWidget(centralStackedWidget);

    /* Map options dock */
    mapOptionsDock = new QDockWidget;
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

    /* Load map view plugin */
    setMapView(_mapViewPluginManager->instance(_configuration.group("map")->value<string>("viewPlugin")));

    resize(960, 700);
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

    /* Paths */
    string packageDir = ""; /** @todo Directory::home() */
    _configuration.group("paths")->value<string>("packages", &packageDir);

    _configuration.setAutomaticGroupCreation(false);
    _configuration.setAutomaticKeyCreation(false);
}

void MainWindow::setMapView(AbstractMapView* view) {
    if(_mapView) delete _mapView;
    _mapView = view;

    /* Assign map view to second slot in stacked widget */
    centralStackedWidget->addWidget(_mapView);

    /* View exists, connect it */
    if(_mapView) {
        connect(_mapView, SIGNAL(currentCoordinates(Core::Wgs84Coords)), SLOT(currentCoordinates(Core::Wgs84Coords)));
        connect(zoomInAction, SIGNAL(triggered(bool)), _mapView, SLOT(zoomIn()));
        connect(zoomOutAction, SIGNAL(triggered(bool)), _mapView, SLOT(zoomOut()));
    }

    emit mapViewChanged();

    displayMapIfUsable();
}

void MainWindow::setRasterModel(AbstractRasterModel* model) {
    /** @todo Disable Save Raster menu when no writeable format is available at all */

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

    displayMapIfUsable();
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

    displayMapIfUsable();
}

void MainWindow::openRaster() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Select map file"), QString::fromStdString(_configuration.group("paths")->value<string>("packages")));
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

        /* If package cannot be opened or is unusable, destroy that bitch and
           go home */
        if(firstSupport->addPackage(filename.toStdString()) == -1 || !firstSupport->isUsable()) {
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

void MainWindow::displayMapIfUsable() {
    const AbstractRasterModel* model = lockRasterModelForRead();
    string name = model ? model->name() : "";
    bool isUsable = model ? model->isUsable() : false;
    unlockRasterModel();

    /* Display map view, map options dock */
    if(_mapView && isUsable) {
        /* Enable menus */
        saveRasterMenu->setDisabled(false);
        closeRasterAction->setDisabled(false);
        mapMenu->setDisabled(false);

        /* Update action in "save raster" menu */
        saveRasterAction->setText(tr("Offline %0 package").arg(
            QString::fromStdString(_rasterModelPluginManager->metadata(name)->name())
        ));
        if(model->features() & AbstractRasterModel::WriteableFormat)
            saveRasterAction->setDisabled(false);
        else
            saveRasterAction->setDisabled(true);

        /* Show map view, show map options dock */
        centralStackedWidget->setCurrentIndex(MAP_VIEW);
        mapOptionsDock->setHidden(false);

        /* Show coordinate status */
        coordinateStatus->setHidden(false);

    /* Display welcome screen */
    } else {
        /* Disable menus */
        saveRasterMenu->setDisabled(true);
        closeRasterAction->setDisabled(true);
        mapMenu->setDisabled(true);

        /* Show welcome screen, hide map options dock */
        centralStackedWidget->setCurrentIndex(WELCOME_SCREEN);
        mapOptionsDock->setHidden(true);

        /* Don't show zeros in coordinate status */
        coordinateStatus->setHidden(true);
    }
}

void MainWindow::createActions() {
    /* Open session */
    openSessionAction = new QAction(QIcon(":/open-session-64.png"), tr("Restore saved session"), this);
    openSessionAction->setDisabled(true);

    /* Open raster map */
    openRasterAction = new QAction(QIcon(":/open-package-64.png"), tr("Open map package"), this);
    connect(openRasterAction, SIGNAL(triggered(bool)), SLOT(openRaster()));

    /* Open online map */
    openOnlineAction = new QAction(QIcon(":/open-online-64.png"), tr("Load online map"), this);

    /* Save raster map */
    saveRasterAction = new QAction(this);
    connect(saveRasterAction, SIGNAL(triggered(bool)), SLOT(saveRaster()));

    /* Close raster map */
    closeRasterAction = new QAction(QIcon(tr(":/close-16.png")), tr("Close map"), this);
    closeRasterAction->setDisabled(true);
    connect(closeRasterAction, SIGNAL(triggered(bool)), SLOT(closeRaster()));

    /* Quit application */
    quitAction = new QAction(QIcon(":/exit-16.png"), tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered(bool)), SLOT(close()));

    /* Move map */
    /** @todo Disable when zooming in/out is not possible */
    zoomInAction = new QAction(tr("Zoom in"), this);
    zoomInAction->setShortcut(Qt::CTRL|Qt::Key_Plus);
    zoomOutAction = new QAction(tr("Zoom out"), this);
    zoomOutAction->setShortcut(Qt::CTRL|Qt::Key_Minus);

    /* Settings menu */
    pluginDialogAction = new QAction(QIcon(":/plugins-16.png"), tr("Plugins"), this);
    configurationDialogAction = new QAction(tr("Configure Kompas"), this);
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
    fileMenu->addAction(openSessionAction);
    fileMenu->addAction(openRasterAction);
    fileMenu->addAction(openOnlineAction);

    /* Open raster map menu */
    openRasterMenu = new QMenu(this);
    openOnlineAction->setMenu(openRasterMenu);

    /* Save raster map menu */
    saveRasterMenu = fileMenu->addMenu(tr("Save map"));
    saveRasterMenu->addAction(saveRasterAction);
    saveRasterMenu->addSeparator();
    saveRasterMenu->setDisabled(true);

    fileMenu->addAction(closeRasterAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    /* Map menu */
    mapMenu = menuBar()->addMenu(tr("Map"));
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

void MainWindow::currentCoordinates(const Kompas::Core::Wgs84Coords& coords) {
    if(!coords.isValid())
        coordinateStatus->setText("");
    else
        coordinateStatus->setText(QString::fromStdString(coords.toString(0, true)));
}

}}
