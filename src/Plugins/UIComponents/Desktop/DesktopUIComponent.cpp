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

#include "DesktopUIComponent.h"

#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QToolButton>
#include <QtGui/QGridLayout>

#include "MainWindow.h"
#include "AbstractMapView.h"

PLUGIN_REGISTER(DesktopUIComponent, Kompas::Plugins::UIComponents::DesktopUIComponent,
                "cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

#define WELCOME_SCREEN 0
#define MAP_VIEW 1

DesktopUIComponent::DesktopUIComponent(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): AbstractUIComponent(manager, plugin) {
    /* Welcome screen, wrapped in another widget so it's nicely centered */
    QFrame* welcomeScreenFrame = new QFrame;
    welcomeScreenFrame->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/welcome-640.png")));
    welcomeScreenFrame->setPalette(palette);

    openSessionButton = new QToolButton(welcomeScreenFrame);
    openSessionButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openSessionButton->setAutoRaise(true);
    openSessionButton->setIconSize(QSize(64, 64));
    openSessionButton->setFixedHeight(96);

    openRasterButton = new QToolButton(welcomeScreenFrame);
    openRasterButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    openRasterButton->setAutoRaise(true);
    openRasterButton->setIconSize(QSize(64, 64));
    openRasterButton->setFixedHeight(96);

    openOnlineButton = new QToolButton(welcomeScreenFrame);
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
    _centralWidget = new QStackedWidget;
    _centralWidget->addWidget(welcomeScreen);

    MainWindow::instance()->resize(960, 700);

    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)), SLOT(rasterModelChanged()));
    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(mapViewChanged()));
}

void DesktopUIComponent::actionAdded(int category, QAction* action) {
    if(action->data().toString() == "openSession") {
        openSessionButton->setDefaultAction(action);
        openSessionButton->setPopupMode(QToolButton::InstantPopup);

    } else if(action->data().toString() == "openRaster") {
        openRasterButton->setDefaultAction(action);

    } else if(action->data().toString() == "openOnline") {
        openOnlineButton->setDefaultAction(action);
        openOnlineButton->setPopupMode(QToolButton::InstantPopup);
    }
}

void DesktopUIComponent::rasterModelChanged() {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    bool isUsable = rasterModel() ? rasterModel()->isUsable() : false;
    rasterModel.unlock();

    /* Show map view, */
    if(MainWindow::instance()->mapView() && isUsable)
        _centralWidget->setCurrentIndex(MAP_VIEW);

    /* Show welcome screen */
    else _centralWidget->setCurrentIndex(WELCOME_SCREEN);
}

void DesktopUIComponent::mapViewChanged() {
    /* View exists - assign map view to second slot in stacked widget */
    if(MainWindow::instance()->mapView()) {
        _centralWidget->addWidget(MainWindow::instance()->mapView());

        /* Show the map view, if map is usable */
        rasterModelChanged();
    }
}

}}}
