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

#include "AbstractMapView.h"

#include <QtCore/QMetaType>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>

#include "TileDataThread.h"
#include "MainWindow.h"

using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

AbstractMapView::AbstractMapView(Corrade::PluginManager::AbstractPluginManager* manager, const std::string& plugin): Plugin(manager, plugin) {
    tileDataThread = new TileDataThread(this);

    /* Context menu */
    QMenu* coordinatesMenu = new QMenu(this);
    coordinatesMenu->addAction(tr("Copy to clipboard"), this, SLOT(copyCoordsToClipboard()));
    contextMenu = new QMenu(this);
    coordinatesAction = contextMenu->addAction("");
    coordinatesAction->setMenu(coordinatesMenu);

    qRegisterMetaType<Core::Zoom>("Core::Zoom");
    qRegisterMetaType<Core::TileCoords>("Core::TileCoords");

    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)), SLOT(updateRasterModel(const Core::AbstractRasterModel*)));

    connect(tileDataThread, SIGNAL(tileData(QString,Core::Zoom,Core::TileCoords,QByteArray)),
            SLOT(tileData(QString,Core::Zoom,Core::TileCoords,QByteArray)));
    connect(tileDataThread, SIGNAL(tileLoading(QString,Core::Zoom,Core::TileCoords)),
            SLOT(tileLoading(QString,Core::Zoom,Core::TileCoords)));
    connect(tileDataThread, SIGNAL(tileNotFound(QString,Core::Zoom,Core::TileCoords)),
            SLOT(tileNotFound(QString,Core::Zoom,Core::TileCoords)));

    tileDataThread->start();
}

void AbstractMapView::contextMenuEvent(QContextMenuEvent* event) {
    LatLonCoords c = coords(event->pos());
    if(!c.isValid()) return;

    /* Save coordinates (in full precision) for Copy to clipboard action */
    lastCoordsForClipboard = QString::fromStdString(c.toString());

    /* Set coordinate action text and show menu */
    coordinatesAction->setText(tr("Coordinates: %0").arg(QString::fromStdString(c.toString(0, true))));
    contextMenu->move(mapToGlobal(event->pos()));
    contextMenu->show();
}

bool AbstractMapView::isReady() {
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    return rasterModel() && rasterModel()->isUsable();
}

void AbstractMapView::copyCoordsToClipboard() {
    if(lastCoordsForClipboard.isEmpty()) return;

    qApp->clipboard()->setText(lastCoordsForClipboard);

    lastCoordsForClipboard.clear();
}

}}
