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

#include <QtCore/QTimer>

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

    QTimer::singleShot(0, this, SIGNAL(mapResized()));
}

void MapView::drawForeground(QPainter* painter, const QRectF& _rect) {
    /* If the copyright is empty, nothing to do */
    if(_copyright->isEmpty()) return;

    /* The background rect has margin 1px, the text has left and right margin
       2px, top and bottom margin 1px. */
    QRect textBounds = QRect(QPoint(3, 2), contentsRect().size()-QSize(6, 4));

    /* Compute text rectangle from font metrics */
    QFontMetrics metrics = painter->fontMetrics();
    QRect textRect = metrics.boundingRect(textBounds, 0, *_copyright);

    /* Pixmap for rendering copyright */
    QPixmap copyright(textRect.width()+6, textRect.height()+4);
    copyright.fill(QColor(0, 0, 0, 0));
    QPainter copyrightPainter(&copyright);

    /* Draw background with light white brush and no pen */
    copyrightPainter.setBrush(QBrush(QColor(255, 255, 255, 160)));
    copyrightPainter.setPen(Qt::NoPen);
    copyrightPainter.drawRoundedRect(1, 1, copyright.width()-2, copyright.height()-2, 2, 2);

    /* Draw text */
    copyrightPainter.setPen(QPen(Qt::black));
    copyrightPainter.drawText(textRect, *_copyright);

    /* Draw the copyright onto the widget */
    QPoint topLeft = mapToScene(0, 0).toPoint() +
        QPoint(contentsRect().width() - copyright.width(), contentsRect().height() - copyright.height());
    painter->drawPixmap(topLeft, copyright);
}

}}
