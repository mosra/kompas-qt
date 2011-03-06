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
#include <QtGui/QInputDialog>

#include "Utility/Directory.h"
#include "MainWindowConfigure.h"
#include "PluginManager.h"
#include "PluginDialog.h"
#include "TileDataThread.h"
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
#include "AboutDialog.h"
#include "SessionMenuView.h"

#define WELCOME_SCREEN 0
#define MAP_VIEW 1

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

MainWindow* MainWindow::_instance;

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), _configuration(Directory::join(Directory::configurationDir("Kompas"), "kompas.conf")), sessionManager(QString::fromStdString(Directory::join(Directory::configurationDir("Kompas"), "sessions.conf"))), _mapView(0), _rasterModel(0) {
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

    createActions();
    createMenus();
    createUI();

    /* Sessions */
    currentSessionChange();
    connect(&sessionManager, SIGNAL(currentChanged(uint)), SLOT(currentSessionChange()));

    /* Welcome screen, wrapped in another widget so it's nicely centered */
    QFrame* welcomeScreenFrame = new QFrame;
    welcomeScreenFrame->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/welcome-640.png")));
    welcomeScreenFrame->setPalette(palette);

    QToolButton* openSessionButton = new QToolButton(this);
    openSessionButton->setDefaultAction(openSessionAction);
    openSessionButton->setPopupMode(QToolButton::InstantPopup);
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

    /* Status bar with coordinates */
    coordinateStatus = new QLabel;
    statusBar()->addPermanentWidget(coordinateStatus);

    /* Load map view plugin */
    setMapView(_pluginManagerStore->mapViews()->manager()->instance(_configuration.group("map")->value<string>("viewPlugin")));

    resize(960, 700);
}

MainWindow::~MainWindow() {
    sessionManager.save();
}

void MainWindow::loadDefaultConfiguration() {
    _configuration.setAutomaticGroupCreation(true);
    _configuration.setAutomaticKeyCreation(true);

    /* Plugin dirs */
    string mapViewPluginDir = PLUGIN_MAPVIEW_DIR;
    string celestialBodyPluginDir = PLUGIN_CELESTIALBODY_DIR;
    string projectionPluginDir = PLUGIN_PROJECTION_DIR;
    string rasterModelPluginDir = PLUGIN_RASTERMODEL_DIR;
    string toolPluginDir = PLUGIN_TOOL_DIR;
    string uiComponentPluginDir = PLUGIN_UICOMPONENT_DIR;

    /* On Win32 add program directory to the dirs to make them absolute */
    #ifdef _WIN32
    string programPath = QApplication::applicationDirPath().toStdString();
    mapViewPluginDir = programPath + mapViewPluginDir;
    celestialBodyPluginDir = programPath + celestialBodyPluginDir;
    projectionPluginDir = programPath + projectionPluginDir;
    rasterModelPluginDir = programPath + rasterModelPluginDir;
    toolPluginDir = programPath + toolPluginDir;
    uiComponentPluginDir = programPath + uiComponentPluginDir;
    #endif

    _configuration.group("plugins")->group("mapViews")->value<string>("__dir", &mapViewPluginDir);
    _configuration.group("plugins")->group("celestialBodies")->value<string>("__dir", &celestialBodyPluginDir);
    _configuration.group("plugins")->group("projections")->value<string>("__dir", &projectionPluginDir);
    _configuration.group("plugins")->group("rasterModels")->value<string>("__dir", &rasterModelPluginDir);
    _configuration.group("plugins")->group("tools")->value<string>("__dir", &toolPluginDir);
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

    /* View exists */
    if(_mapView) {
        /* Assign map view to second slot in stacked widget */
        centralStackedWidget->addWidget(_mapView);

        connect(_mapView, SIGNAL(currentCoordinates(Core::LatLonCoords)), SLOT(currentCoordinates(Core::LatLonCoords)));
    }

    emit mapViewChanged();

    displayMapIfUsable();
}

void MainWindow::setRasterModel(AbstractRasterModel* model) {
    /** @todo @c VERSION-0.1.1 Disable Save Raster menu when no writeable format is available at all */

    rasterModelLock.lockForWrite();
    AbstractRasterModel* oldRasterModel = _rasterModel;
    _rasterModel = model;
    rasterModelLock.unlock();

    /* Update save raster menu to avoid showing the same plugin twice */
    saveRasterMenuView->update();

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

void MainWindow::openRaster() {
    PluginManager<AbstractRasterModel>* rasterModelPluginManager = _pluginManagerStore->rasterModels()->manager();

    /* Compose filter from all loaded plugins */
    QString filter;
    vector<string> plugins = rasterModelPluginManager->pluginList();
    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
        /* Skip not loaded plugins */
        if(!(rasterModelPluginManager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)))
            continue;

        /* Instance of the model */
        AbstractRasterModel* instance = rasterModelPluginManager->instance(*it);
        if(!instance) continue;

        vector<string> extensions = instance->fileExtensions();
        if(extensions.empty()) {
            delete instance;
            continue;
        }

        filter += QString::fromStdString(*instance->metadata()->name()) + " (";

        for(vector<string>::const_iterator it = extensions.begin(); it != extensions.end(); ++it) {
            filter += QString::fromStdString(*it) + " ";
        }

        filter = filter.left(filter.length()-1) + ");;";

        delete instance;
    }

    filter += tr("All files (*)");

    QString filename = QFileDialog::getOpenFileName(this, tr("Select map file"), QString::fromStdString(_configuration.group("paths")->value<string>("packages")), filter);
    if(filename.isEmpty()) return;

    /* Try to open the package with current model */
    /** @todo Disable online maps? */
    if(_rasterModel && _rasterModel->addPackage(filename.toStdString()) != -1) {
        /** @todo Open only if better plugin doesn't exist */
        _rasterPackageModel->reload();
        _rasterLayerModel->reload();
        _rasterOverlayModel->reload();
        _rasterZoomModel->reload();

        emit rasterModelChanged();

    /* Else try all plugins */
    } else {
        std::ifstream i(filename.toUtf8().constData());
        AbstractRasterModel* firstSupport = 0;
        int state = AbstractRasterModel::NotSupported;

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

        /* Package format is deprecated */
        if(state == AbstractRasterModel::DeprecatedSupport) {
            MessageBox::warning(this, tr("Deprecated file format"), tr("The package has deprecated format that probably won't be supported in future versions. Please save the package to a newer format."));
        }
    }
}

void MainWindow::saveRaster() {
    string plugin;

    Locker<const AbstractRasterModel> rasterModel = rasterModelForRead();
    if(rasterModel()) plugin = rasterModel()->plugin();
    rasterModel.unlock();

    if(plugin.empty()) return;

    SaveRasterWizard wizard(plugin);
    wizard.exec();
}

void MainWindow::displayMapIfUsable() {
    Locker<const AbstractRasterModel> rasterModel = rasterModelForRead();
    QString name = rasterModel() ? QString::fromStdString(*rasterModel()->metadata()->name()) : "";
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    bool isWriteable = rasterModel() ? (rasterModel()->features() & AbstractRasterModel::WriteableFormat) : false;
    rasterModel.unlock();

    /* Display map view, map options dock */
    if(_mapView && isUsable) {
        /* Enable menus */
        saveRasterMenu->setDisabled(false);
        closeRasterAction->setDisabled(false);

        /* Update action in "save raster" menu */
        saveCurrentRasterAction->setText(tr("Offline %0 package").arg(name));
        if(isWriteable)
            saveCurrentRasterAction->setDisabled(false);
        else
            saveCurrentRasterAction->setDisabled(true);

        /* Show map view, show dock widgets */
        centralStackedWidget->setCurrentIndex(MAP_VIEW);
        mapOptionsDock->setHidden(false);
        foreach(QDockWidget* widget, _dockWidgets)
            widget->setHidden(false);

        /* Show coordinate status */
        coordinateStatus->setHidden(false);

    /* Display welcome screen */
    } else {
        /* Disable menus */
        saveRasterMenu->setDisabled(true);
        closeRasterAction->setDisabled(true);

        /* Show welcome screen, hide dock widgets */
        centralStackedWidget->setCurrentIndex(WELCOME_SCREEN);
        mapOptionsDock->setHidden(true);
        foreach(QDockWidget* widget, _dockWidgets)
            widget->setHidden(true);

        /* Don't show zeros in coordinate status */
        coordinateStatus->setHidden(true);
    }
}

void MainWindow::createActions() {
    /* Open session */
    QIcon openSessionIcon;
    openSessionIcon.addFile(":/open-session-16.png");
    openSessionIcon.addFile(":/open-session-64.png");
    openSessionAction = new QAction(openSessionIcon, tr("Restore saved session"), this);
    _actions.insert(AbstractUIComponent::Sessions, openSessionAction);

    /* Create new session */
    newSessionAction = new QAction(tr("Create new session"), this);
    connect(newSessionAction, SIGNAL(triggered(bool)), SLOT(newSession()));
    _actions.insert(AbstractUIComponent::Sessions, newSessionAction);

    /* Rename current session */
    renameSessionAction = new QAction(tr("Rename current session"), this);
    connect(renameSessionAction, SIGNAL(triggered(bool)), SLOT(renameSession()));
    _actions.insert(AbstractUIComponent::Sessions, renameSessionAction);

    /* Delete current session */
    deleteSessionAction = new QAction(tr("Delete current session"), this);
    connect(deleteSessionAction, SIGNAL(triggered(bool)), SLOT(deleteSession()));
    _actions.insert(AbstractUIComponent::Sessions, deleteSessionAction);

    /* Open raster map */
    QIcon openPackageIcon;
    openPackageIcon.addFile(":/open-package-16.png");
    openPackageIcon.addFile(":/open-package-64.png");
    openRasterAction = new QAction(openPackageIcon, tr("Open map package"), this);
    connect(openRasterAction, SIGNAL(triggered(bool)), SLOT(openRaster()));
    _actions.insert(AbstractUIComponent::Maps, openRasterAction);

    /* Open online map */
    QIcon openOnlineIcon;
    openOnlineIcon.addFile(":/open-online-16.png");
    openOnlineIcon.addFile(":/open-online-64.png");
    openOnlineAction = new QAction(openOnlineIcon, tr("Load online map"), this);
    _actions.insert(AbstractUIComponent::Maps, openOnlineAction);

    /* Save raster map */
    saveRasterAction = new QAction(QIcon(":/save-16.png"), tr("Save map"), this);
    _actions.insert(AbstractUIComponent::Maps, saveRasterAction);

    /* Save raster map to current model */
    saveCurrentRasterAction = new QAction(this);
    connect(saveCurrentRasterAction, SIGNAL(triggered(bool)), SLOT(saveRaster()));

    /* Close raster map */
    closeRasterAction = new QAction(QIcon(":/close-16.png"), tr("Close map"), this);
    closeRasterAction->setDisabled(true);
    connect(closeRasterAction, SIGNAL(triggered(bool)), SLOT(closeRaster()));
    _actions.insert(AbstractUIComponent::Maps, closeRasterAction);

    /* Settings menu */
    pluginDialogAction = new QAction(QIcon(":/plugins-16.png"), tr("Plugins"), this);
    connect(pluginDialogAction, SIGNAL(triggered(bool)), SLOT(pluginDialog()));
    _actions.insert(AbstractUIComponent::Settings, pluginDialogAction);

    /* About */
    aboutAction = new QAction(QIcon(":/logo-16.png"), tr("About Kompas"), this);
    aboutAction->setStatusTip(tr("Show information about this application"));
    connect(aboutAction, SIGNAL(triggered(bool)), SLOT(aboutDialog()));
    _actions.insert(AbstractUIComponent::Help, aboutAction);

    /* About Qt */
    aboutQtAction = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    connect(aboutQtAction, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    _actions.insert(AbstractUIComponent::Help, aboutQtAction);
}

void MainWindow::createMenus() {
    /* Session list menu */
    sessionMenu = new QMenu(this);
    openSessionAction->setMenu(sessionMenu);
    sessionMenu->addAction(newSessionAction);
    sessionMenu->addSeparator();
    new SessionMenuView(&sessionManager, sessionMenu, this);

    /* Open raster map menu */
    openRasterMenu = new QMenu(this);
    openOnlineAction->setMenu(openRasterMenu);
    OpenRasterMenuView* openRasterMenuView = new OpenRasterMenuView(pluginManagerStore()->rasterModels()->manager(), openRasterMenu, 0, this);
    openRasterMenuView->update();

    /* Save raster map menu */
    saveRasterMenu = new QMenu(this);
    saveRasterAction->setMenu(saveRasterMenu);
    saveRasterMenu->addAction(saveCurrentRasterAction);
    saveRasterMenu->addSeparator();
    saveRasterMenu->setDisabled(true);
    saveRasterMenuView = new SaveRasterMenuView(pluginManagerStore()->rasterModels()->manager(), saveRasterMenu, 0, this);
    saveRasterMenuView->update();

    /* Tools menu */
    toolsMenu = menuBar()->addMenu(tr("Tools"));
    ToolPluginMenuView* toolPluginMenuView = new ToolPluginMenuView(this, pluginManagerStore()->tools()->manager(), toolsMenu, 0, this);
    toolPluginMenuView->update();
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

void MainWindow::pluginDialog() {
    PluginDialog* dialog = new PluginDialog(this);
    dialog->show();
}

void MainWindow::aboutDialog() {
    AboutDialog* dialog = new AboutDialog(this);
    dialog->show();
}

void MainWindow::currentSessionChange() {
    /* Window title, disable/enable menu items */
    if(!sessionManager.isLoaded() || sessionManager.current() == 0) {
        setWindowTitle(QString("%0 %1").arg(tr("Kompas")).arg(KOMPAS_QT_VERSION));
        renameSessionAction->setDisabled(true);
        deleteSessionAction->setDisabled(true);
    } else {
        setWindowTitle(QString("[ %1 ] - %0").arg(tr("Kompas")).arg(sessionManager.names()[sessionManager.current()-1]));
        renameSessionAction->setDisabled(false);
        deleteSessionAction->setDisabled(false);
    }
}

void MainWindow::newSession() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("New session"), tr("Enter new session name:"), QLineEdit::Normal, tr("New session"), &ok);
    if(!ok) return;

    unsigned int id = sessionManager.newSession(name);
    sessionManager.load(id);
}

void MainWindow::renameSession() {
    if(sessionManager.current() == 0) return;

    bool ok;
    QString name = QInputDialog::getText(this, tr("Rename session"), tr("Enter new session name:"), QLineEdit::Normal, sessionManager.names()[sessionManager.current()-1], &ok);
    if(!ok) return;

    sessionManager.renameSession(sessionManager.current(), name);
}

void MainWindow::deleteSession() {
    if(sessionManager.current() == 0) return;

    if(MessageBox::question(this, tr("Delete session"), tr("Are you sure you want to delete session '%0'?").arg(sessionManager.names()[sessionManager.current()-1]), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;

    sessionManager.deleteSession(sessionManager.current());
    sessionManager.load(0);
}

void MainWindow::currentCoordinates(const Kompas::Core::LatLonCoords& coords) {
    if(!coords.isValid())
        coordinateStatus->setText("");
    else
        coordinateStatus->setText(QString::fromStdString(coords.toString(0, true)));
}

}}
