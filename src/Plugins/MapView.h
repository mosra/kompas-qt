#ifndef Map2X_QtGui_Plugins_MapView_h
#define Map2X_QtGui_Plugins_MapView_h
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
 * @brief Class Map2X::QtGui::Plugins::MapView
 */

#include <QtGui/QGraphicsView>

namespace Map2X { namespace QtGui { namespace Plugins {

/** @brief Map view area in GraphicsMapView */
class MapView: public QGraphicsView {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         */
        inline MapView(QWidget* parent = 0): QGraphicsView(parent) {}

    protected:
        /**
         * @brief Mouse move event
         * @param event         Event
         *
         * Emits mapMoved() signal.
         */
        inline virtual void mouseMoveEvent(QMouseEvent* event) {
            QGraphicsView::mouseMoveEvent(event);

            emit mapMoved();
        }

        /**
         * @brief Resize event
         * @param event         Event
         *
         * Emits mapResized() signal.
         */
        virtual void resizeEvent(QResizeEvent* event) {
            QGraphicsView::resizeEvent(event);

            emit mapResized();
        }

    signals:
        void mapMoved();        /**< @brief Map moved */
        void mapResized();      /**< @brief Map resized */
};

}}}

#endif
