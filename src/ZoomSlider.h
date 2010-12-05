#ifndef Kompas_QtGui_ZoomSlider_h
#define Kompas_QtGui_ZoomSlider_h
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

#include <QtGui/QSlider>

#include "AbstractRasterModel.h"

namespace Kompas { namespace QtGui {

class ZoomSlider: public QSlider {
    Q_OBJECT

    public:
        ZoomSlider(QWidget* parent = 0);

    public slots:
        void updateMapView();
        void updateRasterModel();

    private slots:
        void updateZoom(Core::Zoom z);
        void zoomTo(int value);
};

}}

#endif
