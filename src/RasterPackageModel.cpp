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

#include "RasterPackageModel.h"

#include "MainWindow.h"

using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void RasterPackageModel::reload() {
    beginResetModel();
    packages.clear();

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* All packages */
    if(rasterModel()) for(int i = 0; i != rasterModel()->packageCount(); ++i) {
        Package p;
        p.filename = QString::fromStdString(rasterModel()->packageAttribute(i, AbstractRasterModel::Filename));
        p.name = QString::fromStdString(rasterModel()->packageAttribute(i, AbstractRasterModel::Name));
        p.description = QString::fromStdString(rasterModel()->packageAttribute(i, AbstractRasterModel::Description));
        p.packager = QString::fromStdString(rasterModel()->packageAttribute(i, AbstractRasterModel::Packager));
        packages.append(p);
    }

    rasterModel.unlock();

    endResetModel();
}

QVariant RasterPackageModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.column() >= columnCount() || index.row() >= rowCount() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column()) {
        case Filename:
            return packages.at(index.row()).filename;
        case Name:
            if(packages.at(index.row()).name.isEmpty())
                return packages.at(index.row()).filename;
            else
                return packages.at(index.row()).name;
        case Description:
            return packages.at(index.row()).description;
        case Packager:
            return packages.at(index.row()).packager;
    }

    return QVariant();
}

}}
