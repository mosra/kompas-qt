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

#include "TileLayerModel.h"

#include "AbstractRasterModel.h"
#include "AbstractMapView.h"
#include "MainWindow.h"

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

TileLayerModel::TileLayerModel(QObject* parent): QAbstractListModel(parent) { reload(); }

void TileLayerModel::reload() {
    beginResetModel();
    layers.clear();

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    if(rasterModel) {
        /* All available layers */
        vector<string> _layers = rasterModel->layers();
        for(vector<string>::const_iterator it = _layers.begin(); it != _layers.end(); ++it)
            layers.append(QString::fromStdString(*it));
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant TileLayerModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount()) return QVariant();

    if(role == Qt::DisplayRole) return layers.at(index.row());

    return QVariant();
}

}}
