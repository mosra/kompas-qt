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

#include "RasterOverlayModel.h"

#include "MainWindow.h"

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void RasterOverlayModel::reload() {
    beginResetModel();
    overlays.clear();

    AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForWrite();

    if(rasterModel) {
        /* All available layers */
        vector<string> _overlays = rasterModel->overlays();
        for(vector<string>::const_iterator it = _overlays.begin(); it != _overlays.end(); ++it) {
            Overlay o;
            o.name = QString::fromStdString(*it);
            o.translated = rasterModel->layerName(*it);
            overlays.append(o);
        }
    }

    MainWindow::instance()->unlockRasterModel();

    endResetModel();
}

QVariant RasterOverlayModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() > 1 || index.row() >= rowCount() || role != Qt::DisplayRole)
        return QVariant();

    const Overlay& o = overlays.at(index.row());

    switch(index.column()) {
        case Name: return o.name;
        case Translated: return QString::fromStdString(*o.translated);
    }

    return QVariant();
}

QModelIndex RasterOverlayModel::find(const QString& overlay) {
    for(int i = 0; i != overlays.size(); ++i) if(overlays[i].name == overlay)
        return index(i, 0);

    return QModelIndex();
}

}}
