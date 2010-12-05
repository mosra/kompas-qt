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

#include "ZoomSlider.h"

#include "MainWindow.h"

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

ZoomSlider::ZoomSlider(QWidget* parent): QSlider(parent) {
    setTickPosition(QSlider::TicksBothSides);
    setTickInterval(3);
    setSingleStep(1);
    setPageStep(1);
    setMinimum(0);

    connect(this, SIGNAL(valueChanged(int)), SLOT(zoomTo(int)));

    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(updateMapView()));
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged()), SLOT(updateRasterModel()));
}

void ZoomSlider::updateMapView() {
    connect(MainWindow::instance()->mapView(), SIGNAL(zoomChanged(Core::Zoom)), SLOT(updateZoom(Core::Zoom)));
}

void ZoomSlider::updateRasterModel() {
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    setMaximum(model->zoomLevels().size()-1);
    MainWindow::instance()->unlockRasterModel();
}

void ZoomSlider::updateZoom(Core::Zoom z) {
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    vector<Zoom> levels = model->zoomLevels();
    MainWindow::instance()->unlockRasterModel();

    /* Find the zoom in vector */
    int value = -1;
    for(size_t i = 0; i != levels.size(); ++i) if(levels[i] == z) {
        value = i; /** @todo Faster O(log n) searching */
        break;
    }

    setValue(value);
}

void ZoomSlider::zoomTo(int value) {
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    Zoom z = model->zoomLevels()[value];
    MainWindow::instance()->unlockRasterModel();

    MainWindow::instance()->mapView()->zoomTo(z);
}

}}
