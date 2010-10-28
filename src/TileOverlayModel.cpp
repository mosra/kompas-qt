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

#include "TileOverlayModel.h"

#include "AbstractRasterModel.h"
#include "AbstractMapView.h"
#include "MainWindow.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

TileOverlayModel::TileOverlayModel(QObject* parent): QAbstractListModel(parent) { reload(); }

void TileOverlayModel::reload() {
    beginResetModel();
    overlays.clear();

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    if(rasterModel) {
        /* All available layers */
        vector<string> _overlays = rasterModel->overlays();
        for(vector<string>::const_iterator it = _overlays.begin(); it != _overlays.end(); ++it)
            overlays.append(QString::fromStdString(*it));
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant TileOverlayModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount()) return QVariant();

    if(role == Qt::DisplayRole) return overlays.at(index.row());

    return QVariant();
}

}}
