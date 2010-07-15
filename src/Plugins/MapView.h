#ifndef Map2X_QtGui_MapView_h
#define Map2X_QtGui_MapView_h
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

/** @file
 * @brief Class Map2X::QtGui::MapView
 */

#include <QtGui/QGraphicsScene>

#include "../AbstractMapView.h"

class QGraphicsView;

namespace Map2X {

namespace QtGui { namespace Plugins {

/** @brief Map viewer using QGraphicsView */
class MapView: public AbstractMapView {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param f         Window flags
         */
        MapView(QWidget* parent = 0, Qt::WindowFlags f = 0);

    private:
        QGraphicsView* view;
        QGraphicsScene map;

    private slots:
        virtual void refresh();
};

}}}

PLUGIN_REGISTER_STATIC(MapView, Map2X::QtGui::Plugins::MapView, "cz.mosra.Map2X.QtGui.AbstractMapView/0.1")
PLUGIN_SET_NAME("Map viewer using QGraphicsView")
PLUGIN_FINISH

#endif
