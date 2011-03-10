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

#include "ZoomSlider.h"

#include "MainWindow.h"
#include "AbstractMapView.h"

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

ZoomSlider::ZoomSlider(QWidget* parent): QSlider(parent) {
    setTickPosition(QSlider::TicksBothSides);
    setTickInterval(3);
    setSingleStep(1);
    setPageStep(1);
    setMinimum(0);
    setMaximum(0);

    connect(this, SIGNAL(valueChanged(int)), SLOT(zoomTo(int)));

    connect(MainWindow::instance(), SIGNAL(mapViewChanged()), SLOT(updateMapView()));
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged()), SLOT(updateRasterModel()));
}

void ZoomSlider::updateMapView() {
    AbstractMapView* view = MainWindow::instance()->mapView();
    if(!view) return;
    connect(view, SIGNAL(zoomChanged(Core::Zoom)), SLOT(updateZoom(Core::Zoom)));
}

void ZoomSlider::updateRasterModel() {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    if(rasterModel() && rasterModel()->zoomLevels().size() > 1) {
        setDisabled(false);
        setMinimum(*rasterModel()->zoomLevels().begin());
        setMaximum(*--rasterModel()->zoomLevels().end());
    } else {
        setDisabled(true);
        setMinimum(0);
        setMaximum(0);
    }
}

void ZoomSlider::updateZoom(Core::Zoom z) {
    setValue(static_cast<int>(z));
}

void ZoomSlider::zoomTo(int value) {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    set<Zoom> levels;
    if(rasterModel()) levels = rasterModel()->zoomLevels();
    rasterModel.unlock();

    Zoom wanted = static_cast<Zoom>(value);
    set<Zoom>::const_iterator lower = levels.lower_bound(wanted);
    set<Zoom>::const_iterator upper = levels.upper_bound(wanted);

    if(lower == levels.end() && upper == levels.end()) return;

    /* Wanted zoom exists */
    if(lower != levels.end() && *lower == wanted) {
        AbstractMapView* view = MainWindow::instance()->mapView();
        if(view) view->zoomTo(wanted);
        return;
    }

    /* Lower is nearer */
    if(upper == levels.end() || (lower != levels.end() && wanted - *lower < *upper - wanted))
        setValue(static_cast<int>(*lower));

    /* Upper is nearer */
    else setValue(static_cast<int>(*upper));
}

}}}
