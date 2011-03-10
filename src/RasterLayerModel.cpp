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

#include "RasterLayerModel.h"

#include "MainWindow.h"
#include "AbstractRasterModel.h"

using namespace std;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void RasterLayerModel::reload() {
    beginResetModel();
    layers.clear();

    Locker<AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForWrite();

    if(rasterModel()) {
        /* All available layers */
        vector<string> _layers = rasterModel()->layers();
        for(vector<string>::const_iterator it = _layers.begin(); it != _layers.end(); ++it) {
            Layer l;
            l.name = QString::fromStdString(*it);
            l.translated = rasterModel()->layerName(*it);
            layers.append(l);
        }
    }

    rasterModel.unlock();

    endResetModel();
}

QVariant RasterLayerModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() > 1 || index.row() >= rowCount() || role != Qt::DisplayRole)
        return QVariant();

    const Layer& l = layers.at(index.row());

    switch(index.column()) {
        case Name: return l.name;
        case Translated:
            if(l.translated->empty()) return l.name;
            return QString::fromStdString(*l.translated);
    }

    return QVariant();
}

QModelIndex RasterLayerModel::find(const QString& layer) {
    for(int i = 0; i != layers.size(); ++i) if(layers[i].name == layer)
        return index(i, 0);

    return QModelIndex();
}

}}
