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

#include "MapView.h"

#include <QtCore/QDebug>

namespace Kompas { namespace Plugins {

void MapView::mouseMoveEvent(QMouseEvent* event) {
    if(!event->buttons()) event->ignore();
    else {
        QGraphicsView::mouseMoveEvent(event);
        emit mapMoved();
    }
}

void MapView::wheelEvent(QWheelEvent* event) {
    event->accept();

    if(event->delta() > 0) emit zoomIn(event->pos());
    else emit zoomOut(event->pos());
}

void MapView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);

    emit mapResized();
}

void MapView::drawForeground(QPainter* painter, const QRectF& _rect) {
    /* If the copyright is empty, nothing to do */
    if(_copyright->isEmpty()) return;

    /* The background rect has margin 1px, the text has left and right margin
       2px, top and bottom margin 1px. */
    QRect textBounds = QRect(mapToScene(3, 2).toPoint(), contentsRect().size()-QSize(4, 3));

    /* Compute text rectangle from font metrics */
    QFontMetrics metrics = painter->fontMetrics();
    QRect textRect = metrics.boundingRect(textBounds, Qt::AlignRight|Qt::AlignBottom, *_copyright);

    /* Rectangle for background */
    QRect backgroundRect = textRect;
    backgroundRect.moveTopLeft(backgroundRect.topLeft()-QPoint(2, 1));
    backgroundRect.moveBottomRight(backgroundRect.bottomRight()+QPoint(2, 1));

    /* Save previous brush and pen */
    QBrush textBrush = painter->brush();
    QPen textPen = painter->pen();

    /* Draw background with light white brush and no pen, then revert it back */
    painter->setBrush(QBrush(QColor(255, 255, 255, 160)));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(backgroundRect, 2, 2);
    painter->setPen(textPen);
    painter->setBrush(textBrush);

    /* Draw text */
    painter->drawText(textRect, Qt::AlignRight|Qt::AlignBottom, *_copyright);
}

}}
