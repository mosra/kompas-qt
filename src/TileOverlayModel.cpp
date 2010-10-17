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

#include "AbstractTileModel.h"
#include "AbstractMapView.h"
#include "MainWindow.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

TileOverlayModel::TileOverlayModel(AbstractMapView** _mapView, int flags, QObject* parent): QAbstractListModel(parent), mapView(_mapView), _flags(flags) { reload(); }

void TileOverlayModel::reload() {
    beginResetModel();
    overlays.clear();
    loaded.clear();

    const AbstractTileModel* tileModel = MainWindow::instance()->lockTileModelForRead();

    if(*mapView && tileModel) {
        /* Only loaded overlays */
        if(_flags & LoadedOnly) {
            overlays.append((*mapView)->overlays());
            loaded.fill(true, overlays.size());

        /* All available overlays */
        } else {
            /* Make sure loadedOverlays bitarray is as large as overlays list */
            loaded.fill(false, overlays.size());

            vector<string> _overlays = tileModel->overlays();
            QStringList _loaded = (*mapView)->overlays();
            for(vector<string>::const_iterator it = _overlays.begin(); it != _overlays.end(); ++it) {
                overlays.append(QString::fromStdString(*it));

                if(_loaded.contains(overlays.last()))
                    loaded.setBit(overlays.size()-1, true);
            }
        }
    }

    MainWindow::instance()->unlockTileModel();

    endResetModel();
}

QVariant TileOverlayModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount()) return QVariant();

    if(role == Qt::DisplayRole)
        return overlays.at(index.row());
    if(role == Qt::CheckStateRole)
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QVariant();
}

Qt::ItemFlags TileOverlayModel::flags(const QModelIndex& index) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index)|Qt::ItemIsUserCheckable;
}

bool TileOverlayModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount() || role != Qt::CheckStateRole)
        return false;

    /* Remove overlay */
    if(loaded.at(index.row())) {
        if((*mapView)->removeOverlay(overlays.at(index.row()))) {
            loaded.setBit(index.row(), false);
            emit dataChanged(index, index);
            return true;
        }

    /* Add overlay */
    } else {
        if((*mapView)->addOverlay(overlays.at(index.row()))) {
            loaded.setBit(index.row(), true);
            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

}}
