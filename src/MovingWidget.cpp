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

#include "MovingWidget.h"

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>

#include "AbstractMapView.h"
#include "MainWindow.h"

namespace Kompas { namespace QtGui {

MovingWidget::MovingWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
    QGridLayout* layout = new QGridLayout;

    up = new QPushButton(style()->standardIcon(QStyle::SP_ArrowUp), "");
    left = new QPushButton(style()->standardIcon(QStyle::SP_ArrowBack), "");
    right = new QPushButton(style()->standardIcon(QStyle::SP_ArrowForward), "");
    down = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "");
    up->setFlat(true);
    left->setFlat(true);
    right->setFlat(true);
    down->setFlat(true);
    connect(up, SIGNAL(clicked(bool)), SLOT(move()));
    connect(left, SIGNAL(clicked(bool)), SLOT(move()));
    connect(right, SIGNAL(clicked(bool)), SLOT(move()));
    connect(down, SIGNAL(clicked(bool)), SLOT(move()));

    layout->addWidget(up, 0, 1);
    layout->addWidget(left, 1, 0);
    layout->addWidget(right, 1, 2);
    layout->addWidget(down, 2, 1);

    setLayout(layout);
}

void MovingWidget::move() {
    AbstractMapView* view = MainWindow::instance()->mapView();
    if(!view) return;

    int x = 0;
    int y = 0;
    if(sender() == up) y = -64;
    else if(sender() == left) x = -64;
    else if(sender() == right) x = 64;
    else y = 64;

    view->move(x, y);
}

}}
