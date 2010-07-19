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

#include <QtGui/QApplication>
#include <QtGui/QMenuBar>
#include <QtGui/QStyle>

#include "PluginManager/PluginManager.h"
#include "AbstractMapView.h"
#include "MainWindow.h"

using namespace Map2X::Core;
using namespace Map2X::PluginManager;

namespace Map2X { namespace QtGui {

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags), view(0), tileModel(0) {
    setWindowTitle("Map2X");

    statusBar();

    /** @todo Plugin dir */
    viewPluginManager = new ::PluginManager<AbstractMapView>("");
    tilePluginManager = new ::PluginManager<AbstractTileModel>("");

    /** @todo GUI for this */
    view = viewPluginManager->instance("MapView");
    tileModel = tilePluginManager->instance("OpenStreetMap");
    tileModel->setOnline(true);
    view->setTileModel(tileModel);
    view->zoomTo(10);
    view->setLayer("Maplint");

    createActions();
    createMenus();

    setCentralWidget(view);
    resize(800, 600);
}

MainWindow::~MainWindow() {
    delete viewPluginManager;
    delete tilePluginManager;
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
    connect(moveMapAction, SIGNAL(triggered(bool)), SLOT(moveMap()));
    connect(zoomInAction, SIGNAL(triggered(bool)), view, SLOT(zoomIn()));

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

    /* Help menu */
    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::moveMap() {
    /* Move map to some WGS coords */
    view->setCoords(Wgs84Coords(50.08333, 14.46667));
}

}}
