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

#include "MainWindow.h"
#include "AbstractMapView.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MobileCentralWidget::MobileCentralWidget(QWidget* parent): QWidget(parent) {
    _layout = new QHBoxLayout;
    _layout->setContentsMargins(0, 0, 0, 0);
    setLayout(_layout);

    _mapButton = new QPushButton(tr("Map"), this);
    _settingsButton = new QPushButton(tr("Menu"), this);
    _leftButton = new QPushButton(tr("-"), this);
    _rightButton = new QPushButton(tr("+"), this);

    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(mapViewChanged()));
}

void MobileCentralWidget::mapViewChanged() {
    AbstractMapView* view = MainWindow::instance()->mapView();

    /* View doesn't exists, exit */
    if(!view) return;

    _layout->addWidget(view);

    _mapButton->raise();
    _settingsButton->raise();
    _leftButton->raise();
    _rightButton->raise();

    connect(_leftButton, SIGNAL(clicked()), view, SLOT(zoomOut()));
    connect(_rightButton, SIGNAL(clicked()), view, SLOT(zoomIn()));
}

void MobileCentralWidget::positionButtons(const QSize& widgetSize) {
    QRect mapButtonGeometry = _mapButton->geometry();
    mapButtonGeometry.moveTopLeft(QPoint(0, 0));
    _mapButton->setGeometry(mapButtonGeometry);

    QRect settingsButtonGeometry = _settingsButton->geometry();
    settingsButtonGeometry.moveTopRight(QPoint(widgetSize.width(), 0));
    _settingsButton->setGeometry(settingsButtonGeometry);

    QRect leftButtonGeometry = _leftButton->geometry();
    leftButtonGeometry.moveBottomLeft(QPoint(0, widgetSize.height()));
    _leftButton->setGeometry(leftButtonGeometry);

    QRect rightButtonGeometry = _rightButton->geometry();
    rightButtonGeometry.moveBottomRight(QPoint(widgetSize.width(), widgetSize.height()));
    _rightButton->setGeometry(rightButtonGeometry);
}

}}}
