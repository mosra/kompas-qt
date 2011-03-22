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

#include "MainWindow.h"

#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QToolButton>
#include <QtGui/QStackedWidget>

#include "Utility/Directory.h"
#include "MainWindowConfigure.h"
#include "TileDataThread.h"
#include "RasterPackageModel.h"
#include "RasterLayerModel.h"
#include "RasterOverlayModel.h"
#include "RasterZoomModel.h"
#include "PluginManagerStore.h"

#define WELCOME_SCREEN 0
#define MAP_VIEW 1

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

MainWindow* MainWindow::_instance;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(Directory::join(Directory::configurationDir("Kompas"), "kompas.conf")), _sessionManager(QString::fromStdString(Directory::join(Directory::configurationDir("Kompas"), "sessions.conf"))), _mapView(0), _rasterModel(0) {
    _instance = this;

    /* Window icon */
    QIcon icon;
    icon.addFile(":/logo-16.png");
    icon.addFile(":/logo-32.png");
    icon.addFile(":/logo-64.png");
    icon.addFile(":/logo-128.png");
    icon.addFile(":/logo-256.png");
    setWindowIcon(icon);

    /* Load default configuration */
    loadDefaultConfiguration();

    _pluginManagerStore = new PluginManagerStore(_configuration.group("plugins"), this);

    _rasterPackageModel = new RasterPackageModel(this);
    _rasterLayerModel = new RasterLayerModel(this);
    _rasterOverlayModel = new RasterOverlayModel(this);
    _rasterZoomModel = new RasterZoomModel(this);

    TileDataThread::setMaxSimultaenousDownloads(_configuration.group("map")->value<int>("maxSimultaenousDownloads"));

    createUI();

    /* Welcome screen, wrapped in another widget so it's nicely centered */
    QFrame* welcomeScreenFrame = new QFrame;
    welcomeScreenFrame->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/welcome-640.png")));
    welcomeScreenFrame->setPalette(palette);

    QToolButton* openSessionButton = new QToolButton(this);
    openSessionButton->setPopupMode(QToolButton::InstantPopup);
    openSessionButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openSessionButton->setAutoRaise(true);
    openSessionButton->setIconSize(QSize(64, 64));
    openSessionButton->setFixedHeight(96);

    QToolButton* openRasterButton = new QToolButton(this);
    openRasterButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openRasterButton->setAutoRaise(true);
    openRasterButton->setIconSize(QSize(64, 64));
    openRasterButton->setFixedHeight(96);

    QToolButton* openOnlineButton = new QToolButton(this);
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

    /* Load map view plugin */
    setMapView(_pluginManagerStore->mapViews()->manager()->instance(_configuration.group("map")->value<string>("viewPlugin")));

    resize(960, 700);
}

void MainWindow::setWindowTitle(const QString& title) {
    if(title.isEmpty())
        QMainWindow::setWindowTitle(QString("%0 %1").arg(tr("Kompas")).arg(KOMPAS_QT_VERSION));
    else
        QMainWindow::setWindowTitle(QString("[ %1 ] - %0").arg(tr("Kompas")).arg(title));
}

void MainWindow::loadDefaultConfiguration() {
    _configuration.setAutomaticGroupCreation(true);
    _configuration.setAutomaticKeyCreation(true);

    /* Plugin dirs */
    string mapViewPluginDir = PLUGIN_MAPVIEW_DIR;
    string celestialBodyPluginDir = PLUGIN_CELESTIALBODY_DIR;
    string projectionPluginDir = PLUGIN_PROJECTION_DIR;
    string rasterModelPluginDir = PLUGIN_RASTERMODEL_DIR;
    string uiComponentPluginDir = PLUGIN_UICOMPONENT_DIR;

    /* On Win32 add program directory to the dirs to make them absolute */
    #ifdef _WIN32
    string programPath = QApplication::applicationDirPath().toStdString();
    mapViewPluginDir = programPath + mapViewPluginDir;
    celestialBodyPluginDir = programPath + celestialBodyPluginDir;
    projectionPluginDir = programPath + projectionPluginDir;
    rasterModelPluginDir = programPath + rasterModelPluginDir;
    uiComponentPluginDir = programPath + uiComponentPluginDir;
    #endif

    _configuration.group("plugins")->group("mapViews")->value<string>("__dir", &mapViewPluginDir);
    _configuration.group("plugins")->group("celestialBodies")->value<string>("__dir", &celestialBodyPluginDir);
    _configuration.group("plugins")->group("projections")->value<string>("__dir", &projectionPluginDir);
    _configuration.group("plugins")->group("rasterModels")->value<string>("__dir", &rasterModelPluginDir);
    _configuration.group("plugins")->group("uiComponents")->value<string>("__dir", &uiComponentPluginDir);

    /* Plugin for map view */
    string mapViewPlugin = "GraphicsMapView";
    _configuration.group("map")->value("viewPlugin", &mapViewPlugin);

    /* Maximal count of simultaenous downloads */
    unsigned int maxSimultaenousDownloads = 3;
    _configuration.group("map")->value("maxSimultaenousDownloads", &maxSimultaenousDownloads);

    /* Paths */
    string packageDir = Directory::home();
    _configuration.group("paths")->value<string>("packages", &packageDir);

    _configuration.setAutomaticGroupCreation(false);
    _configuration.setAutomaticKeyCreation(false);
}

void MainWindow::setMapView(AbstractMapView* view) {
    if(_mapView) delete _mapView;
    _mapView = view;

    /* View exists - assign map view to second slot in stacked widget */
    if(_mapView)
        centralStackedWidget->addWidget(_mapView);

    emit mapViewChanged();

    displayMapIfUsable();
}

void MainWindow::setRasterModel(AbstractRasterModel* model) {
    /** @todo @c VERSION-0.1.1 Disable Save Raster menu when no writeable format is available at all */

    rasterModelLock.lockForWrite();
    AbstractRasterModel* oldRasterModel = _rasterModel;
    _rasterModel = model;
    rasterModelLock.unlock();

    _rasterPackageModel->reload();
    _rasterLayerModel->reload();
    _rasterOverlayModel->reload();
    _rasterZoomModel->reload();

    emit rasterModelChanged(oldRasterModel);
    delete oldRasterModel;

    displayMapIfUsable();
}

void MainWindow::setOnlineEnabled(bool enabled) {
    rasterModelForWrite()()->setOnline(enabled);

    _rasterPackageModel->reload();
    _rasterLayerModel->reload();
    _rasterOverlayModel->reload();
    _rasterZoomModel->reload();

    emit rasterModelChanged();

    displayMapIfUsable();
}

AbstractRasterModel* MainWindow::rasterModelForFile(const QString& filename, AbstractRasterModel::SupportLevel* supportLevel) {
    PluginManager<AbstractRasterModel>* rasterModelPluginManager = _pluginManagerStore->rasterModels()->manager();

    /* Try to open the package with current model */
    /** @todo Disable online maps? */
    if(_rasterModel && _rasterModel->addPackage(filename.toStdString()) != -1) {
        /** @todo Open only if better plugin doesn't exist */
        _rasterPackageModel->reload();
        _rasterLayerModel->reload();
        _rasterOverlayModel->reload();
        _rasterZoomModel->reload();

        emit rasterModelChanged();

        /** @todo Return really recongized support level */
        *supportLevel = AbstractRasterModel::FullySupported;
        return 0;

    /* Else try all plugins */
    } else {
        std::ifstream i(filename.toUtf8().constData());
        AbstractRasterModel* firstSupport = 0;
        AbstractRasterModel::SupportLevel state = AbstractRasterModel::NotSupported;

        vector<string> plugins = rasterModelPluginManager->pluginList();
        for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
            /* Skip not loaded plugins */
            if(!(rasterModelPluginManager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
                continue;

            /* Instance of the model */
            AbstractRasterModel* instance = rasterModelPluginManager->instance(*it);
            if(!instance) continue;

            /* Skip models which cannot recognize its file format */
            if(!(instance->features() & AbstractRasterModel::SelfRecognizable)) {
                delete instance;
                continue;
            }

            /* Rewind file and try to recognize it */
            i.clear();
            i.seekg(0, ios::beg);
            state = instance->recognizeFile(filename.toStdString(), i);

            /* Try to get full supporting plugin, if not found, go with first
                partially supporting. */
            if(state == AbstractRasterModel::PartiallySupported && !firstSupport)
                firstSupport = instance;
            else if(state == AbstractRasterModel::FullySupported || state == AbstractRasterModel::DeprecatedSupport) {
                delete firstSupport;
                firstSupport = instance;
                break;

            /* Otherwise delete instance and continue with another */
            } else delete instance;
        }

        i.close();

        if(!firstSupport) {
            *supportLevel = AbstractRasterModel::NotSupported;
            return 0;
        }

        *supportLevel = state;
        return firstSupport;
    }
}

void MainWindow::displayMapIfUsable() {
    Locker<const AbstractRasterModel> rasterModel = rasterModelForRead();
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    rasterModel.unlock();

    /* Show map view, show dock widgets */
    if(_mapView && isUsable) {
        centralStackedWidget->setCurrentIndex(MAP_VIEW);
        foreach(QDockWidget* widget, _dockWidgets)
            widget->setHidden(false);

    /* Show welcome screen, hide dock widgets */
    } else {
        centralStackedWidget->setCurrentIndex(WELCOME_SCREEN);
        foreach(QDockWidget* widget, _dockWidgets)
            widget->setHidden(true);
    }
}

void MainWindow::createUI() {
    /* Foreach all loaded UI plugins and instance them */
    PluginManager<AbstractUIComponent>* uiComponentPluginManager = _pluginManagerStore->uiComponents()->manager();
    vector<string> plugins = uiComponentPluginManager->pluginList();
    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(uiComponentPluginManager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))) continue;

        AbstractUIComponent* instance = uiComponentPluginManager->instance(*it);
        instance->setParent(this);

        /* Central widget */
        if(instance->centralWidget()) setCentralWidget(instance->centralWidget());

        /* Dock widget */
        Qt::DockWidgetArea dockWidgetArea = Qt::RightDockWidgetArea;
        if(instance->dockWidget(&dockWidgetArea)) {
            QDockWidget* dock = instance->dockWidget(&dockWidgetArea);
            _dockWidgets << dock;
            addDockWidget(dockWidgetArea, dock);
        }

        /* Menu bar */
        if(instance->menuBar()) setMenuBar(instance->menuBar());

        /* Tool bar */
        Qt::ToolBarArea toolBarArea = Qt::TopToolBarArea;
        if(instance->toolBar(&toolBarArea)) addToolBar(toolBarArea, instance->toolBar(&toolBarArea));

        /* Status bar */
        if(instance->statusBar()) setStatusBar(instance->statusBar());

        /* Actions */
        for(int i = 0; i != 6; ++i) {
            AbstractUIComponent::ActionCategory category = static_cast<AbstractUIComponent::ActionCategory>(i);

            /* Add current actions to component (in QMultiMap are stored in
               reverse order => backward foreach) */
            QList<QAction*> actionsOfCategory = _actions.values(category);
            for(int i = actionsOfCategory.size()-1; i >= 0; --i)
                instance->actionAdded(category, actionsOfCategory[i]);

            /* Add this component's actions to list and other components */
            if(instance->actions(category)) {
                foreach(QAction* action, *instance->actions(category)) {
                    _actions.insert(category, action);
                    emit actionAdded(category, action);
                }
            }
        }

        connect(this, SIGNAL(actionAdded(int,QAction*)), instance, SLOT(actionAdded(int,QAction*)));
    }
}

}}
