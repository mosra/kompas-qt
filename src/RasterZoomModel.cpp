/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "RasterZoomModel.h"

#include "MainWindow.h"

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void RasterZoomModel::reload() {
    beginResetModel();
    z.clear();

    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();

    if(rasterModel) {
        /* All available zoom levels */
        set<Zoom> _z = rasterModel->zoomLevels();
        for(set<Zoom>::const_iterator it = _z.begin(); it != _z.end(); ++it)
            z.append(*it);
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant RasterZoomModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() != 0 || index.row() >= rowCount() || role != Qt::DisplayRole)
        return QVariant();

    return z.at(index.row());
}

QModelIndex RasterZoomModel::find(Zoom zoom) {
    return index(z.indexOf(zoom), 0);
}

}}
