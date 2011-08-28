#ifndef Kompas_Plugins_MapView_h
#define Kompas_Plugins_MapView_h
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

/** @file
 * @brief Class Kompas::Plugins::MapView
 */

#include <QtGui/QGraphicsView>
#include <QtGui/QMouseEvent>

namespace Kompas { namespace Plugins {

/** @brief Map view area in GraphicsMapView */
class MapView: public QGraphicsView {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         */
        inline MapView(QWidget* parent = 0): QGraphicsView(parent) {
            setMouseTracking(true);
        }

    protected:
        /**
         * @brief Mouse move event
         * @param event         Event
         *
         * Emits mapMoved() signal.
         */
        inline void mouseMoveEvent(QMouseEvent* event) {
            if(!event->buttons()) event->ignore();
            else {
                QGraphicsView::mouseMoveEvent(event);
                emit mapMoved();
            }
        }

        /**
         * @brief Mouse wheel event
         * @param event         Event
         *
         * Doesn't move the map, but emits zoomIn() or zoomOut() signal with
         * current cursor position.
         */
        inline void wheelEvent(QWheelEvent* event) {
            event->accept();

            if(event->delta() > 0) emit zoomIn(event->pos());
            else emit zoomOut(event->pos());
        }

        /**
         * @brief Resize event
         * @param event         Event
         *
         * Emits mapResized() signal.
         */
        inline void resizeEvent(QResizeEvent* event) {
            QGraphicsView::resizeEvent(event);

            emit mapResized();
        }

    signals:
        void mapMoved();        /**< @brief Map moved */
        void mapResized();      /**< @brief Map resized */
        void zoomIn(const QPoint& pos = QPoint()); /**< @brief Zooming in requested */
        void zoomOut(const QPoint& pos = QPoint()); /**< @brief Zooming out requested */
};

}}

#endif
