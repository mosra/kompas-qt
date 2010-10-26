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

#include "TileZoomModel.h"

#include "AbstractTileModel.h"
#include "AbstractMapView.h"
#include "MainWindow.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

TileZoomModel::TileZoomModel(QObject* parent): QAbstractListModel(parent) { reload(); }

void TileZoomModel::reload() {
    beginResetModel();
    z.clear();

    const AbstractTileModel* tileModel = MainWindow::instance()->lockTileModelForRead();

    if(tileModel) {
        /* All available zoom levels */
        vector<Zoom> _z = tileModel->zoomLevels();
        for(vector<Zoom>::const_iterator it = _z.begin(); it != _z.end(); ++it)
            z.append(*it);
    }

    MainWindow::instance()->unlockTileModel();

    endResetModel();
}

QVariant TileZoomModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount()) return QVariant();

    if(role == Qt::DisplayRole) return z.at(index.row());

    return QVariant();
}

}}
