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

#include "MobileCentralWidget.h"

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QStackedWidget>
#include <QtGui/QListView>

#include "MainWindow.h"
#include "AbstractMapView.h"
#include "RasterPackageModel.h"
#include "RasterLayerModel.h"
#include <QtGui/QCheckBox>

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MobileCentralWidget::MobileCentralWidget(QWidget* parent): QWidget(parent) {
    _layout = new QHBoxLayout;
    _layout->setContentsMargins(0, 0, 0, 0);
    setLayout(_layout);

    /* Corner buttons */
    _topLeftCorner = new QStackedWidget(this);
    _topRightCorner = new QStackedWidget(this);
    _bottomLeftCorner = new QStackedWidget(this);
    _bottomRightCorner = new QStackedWidget(this);

    /* Default controls */
    _mapButton = new QPushButton(tr("Map"), this);
    connect(_mapButton, SIGNAL(clicked()), SLOT(setMapControlsVisible()));
    _topLeftCorner->addWidget(_mapButton);

    _menuButton = new QPushButton(tr("Menu"), this);
    connect(_menuButton, SIGNAL(clicked()), SLOT(setSettingsControlVisible()));
    _topRightCorner->addWidget(_menuButton);

    _leftButton = new QPushButton(tr("-"), this);
    _bottomLeftCorner->addWidget(_leftButton);

    _rightButton = new QPushButton(tr("+"), this);
    _bottomRightCorner->addWidget(_rightButton);

    /* Map controls */
    _onlineButton = new QPushButton(tr("Online maps"), this);
    _onlineButton->setCheckable(true);
    _onlineButton->setChecked(true);
    connect(_onlineButton, SIGNAL(clicked(bool)), SLOT(setMapOnlineControlWidgetVisible()));
    _topLeftCorner->addWidget(_onlineButton);

    _backButton = new QPushButton(tr("Back"), this);
    connect(_backButton, SIGNAL(clicked(bool)), SLOT(setDefaultControlsVisible()));
    _topRightCorner->addWidget(_backButton);

    _packagesButton = new QPushButton(tr("Packages"), this);
    _packagesButton->setCheckable(true);
    connect(_packagesButton, SIGNAL(clicked(bool)), SLOT(setMapPackagesControlWidgetVisible()));
    _bottomLeftCorner->addWidget(_packagesButton);

    _layersButton = new QPushButton(tr("Map layers"), this);
    _layersButton->setCheckable(true);
    connect(_layersButton, SIGNAL(clicked(bool)), SLOT(setMapLayersControlWidgetVisible()));
    _bottomRightCorner->addWidget(_layersButton);

    /* Settings controls */
    _quitButton = new QPushButton(tr("Quit"), this);
    connect(_quitButton, SIGNAL(clicked()), MainWindow::instance(), SLOT(close()));
    _topLeftCorner->addWidget(_quitButton);

    _configurationButton = new QPushButton(tr("Configuration"), this);
    _configurationButton->setCheckable(true);
    _configurationButton->setChecked(true);
    connect(_configurationButton, SIGNAL(clicked()), SLOT(setMenuConfigurationControlWidgetVisible()));
    _bottomLeftCorner->addWidget(_configurationButton);

    _pluginsButton = new QPushButton(tr("Plugins"), this);
    _pluginsButton->setCheckable(true);
    connect(_pluginsButton, SIGNAL(clicked()), SLOT(setMenuPluginsControlWidgetVisible()));
    _bottomRightCorner->addWidget(_pluginsButton);

    /* Control widgets */
    _mapControlWidget = new QStackedWidget(this);
    _mapControlWidget->setVisible(false);
    _menuControlWidget = new QStackedWidget(this);
    _menuControlWidget->setVisible(false);

    /* Online maps */
    QListView* onlineMaps = new QListView;
    _mapControlWidget->addWidget(onlineMaps);

    /* Map packages */
    QListView* mapPackages = new QListView;
    mapPackages->setModel(MainWindow::instance()->rasterPackageModel());
    _mapControlWidget->addWidget(mapPackages);

    /* Map layers */
    QListView* mapLayers = new QListView;
    mapLayers->setModel(MainWindow::instance()->rasterLayerModel());
    _mapControlWidget->addWidget(mapLayers);

    /* Settings */
    QCheckBox* fullscreen = new QCheckBox(tr("Fullscreen"));
    connect(fullscreen, SIGNAL(clicked(bool)), SLOT(toggleFullscreen()));
    _menuControlWidget->addWidget(fullscreen);

    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(mapViewChanged()));
}

void MobileCentralWidget::setControlsVisible(MobileCentralWidget::Controls controls) {
    _topLeftCorner->setCurrentIndex(controls);
    _topRightCorner->setCurrentIndex(controls == 0 ? 0 : 1);
    _bottomLeftCorner->setCurrentIndex(controls);
    _bottomRightCorner->setCurrentIndex(controls);

    _mapControlWidget->setVisible(false);
    _menuControlWidget->setVisible(false);
    if(controls == Map)  _mapControlWidget->setVisible(true);
    else if(controls == Menu) _menuControlWidget->setVisible(true);
}

void MobileCentralWidget::setMapControlWidgetVisible(MapControlWidget widget) {
    _mapControlWidget->setVisible(true);
    _mapControlWidget->setCurrentIndex(widget);

    _packagesButton->setChecked(false);
    _onlineButton->setChecked(false);
    _layersButton->setChecked(false);

    if(widget == Packages) _packagesButton->setChecked(true);
    else if(widget == Online) _onlineButton->setChecked(true);
    else if(widget == Layers) _layersButton->setChecked(true);
}

void MobileCentralWidget::setMenuControlWidgetVisible(MobileCentralWidget::MenuControlWidget widget) {
    _menuControlWidget->setVisible(true);
    _menuControlWidget->setCurrentIndex(widget);

    if(widget == Configuration) {
        _configurationButton->setChecked(true);
        _pluginsButton->setChecked(false);
    } else if(widget == Plugins) {
        _configurationButton->setChecked(false);
        _pluginsButton->setChecked(true);
    }
}

void MobileCentralWidget::mapViewChanged() {
    AbstractMapView* view = MainWindow::instance()->mapView();

    /* View doesn't exists, exit */
    if(!view) return;

    _layout->addWidget(view);

    _topLeftCorner->raise();
    _topRightCorner->raise();
    _bottomLeftCorner->raise();
    _bottomRightCorner->raise();

    _mapControlWidget->raise();
    _menuControlWidget->raise();

    connect(_leftButton, SIGNAL(clicked()), view, SLOT(zoomOut()));
    connect(_rightButton, SIGNAL(clicked()), view, SLOT(zoomIn()));
}

void MobileCentralWidget::toggleFullscreen() {
    MainWindow* mainWindow = MainWindow::instance();

    if(mainWindow->isFullScreen()) {
        mainWindow->showNormal();
     //   _pluginsButton->setChecked(false);
    } else {
        mainWindow->showFullScreen();
      //  _pluginsButton->setChecked(true);
    }
}

void MobileCentralWidget::positionButtons(const QSize& widgetSize) {
    QRect topLeftCornerGeometry = _topLeftCorner->geometry();
    topLeftCornerGeometry.moveTopLeft(QPoint(0, 0));
    _topLeftCorner->setGeometry(topLeftCornerGeometry);

    QRect topRightCornerGeometry = _topRightCorner->geometry();
    topRightCornerGeometry.moveTopRight(QPoint(widgetSize.width(), 0));
    _topRightCorner->setGeometry(topRightCornerGeometry);

    QRect bottomLeftCornerGeometry = _bottomLeftCorner->geometry();
    bottomLeftCornerGeometry.moveBottomLeft(QPoint(0, widgetSize.height()));
    _bottomLeftCorner->setGeometry(bottomLeftCornerGeometry);

    QRect bottomRightCornerGeometry = _bottomRightCorner->geometry();
    bottomRightCornerGeometry.moveBottomRight(QPoint(widgetSize.width(), widgetSize.height()));
    _bottomRightCorner->setGeometry(bottomRightCornerGeometry);

    _mapControlWidget->setGeometry(widgetSize.width()/8, widgetSize.height()/8, widgetSize.width()*6/8, widgetSize.height()*6/8);
    _menuControlWidget->setGeometry(widgetSize.width()/8, widgetSize.height()/8, widgetSize.width()*6/8, widgetSize.height()*6/8);
}

}}}
