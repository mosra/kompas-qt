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

#include "EditableRasterPackageModel.h"

#include "MainWindow.h"
#include "AbstractRasterModel.h"
#include "RasterPackageModel.h"

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

void EditableRasterPackageModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
}

void EditableRasterPackageModel::reload() {
    beginResetModel();

    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();

    /* If raster model is not available or it doesn't support raster maps, don't show online maps item */
    if(!rasterModel() || !(rasterModel()->features() & AbstractRasterModel::LoadableFromUrl)) {
        rasterModelName.clear();
        online = NotSupported;

    /* Else show it and enable/disable it */
    } else {
        rasterModelName = QString::fromStdString(*rasterModel()->metadata()->name());

        if(rasterModel()->online())   online = Enabled;
        else                    online = Disabled;
    }

    endResetModel();
}

QVariant EditableRasterPackageModel::data(const QModelIndex& _index, int role) const {
    if(online == NotSupported)
        return QAbstractProxyModel::data(_index, role);

    else if(_index.isValid() && _index.row() == 0 &&_index.column() == RasterPackageModel::Name) {
        if(role == Qt::DisplayRole)
            return tr("Online %0").arg(rasterModelName);

        if(role == Qt::CheckStateRole)
            return online == Enabled ? Qt::Checked : Qt::Unchecked;
    }

    return QAbstractProxyModel::data(index(_index.row()-1, _index.column(), _index.parent()), role);
}

Qt::ItemFlags EditableRasterPackageModel::flags(const QModelIndex& _index) const {
    if(online == NotSupported)
        return QAbstractProxyModel::flags(_index);

    else if(online != NotSupported && _index.isValid() && _index.column() == 0 && _index.row() == 0)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index(_index.row()-1, _index.column(), _index.parent()));
}

bool EditableRasterPackageModel::setData(const QModelIndex& _index, const QVariant& value, int role) {
    if(online == NotSupported)
        return QAbstractProxyModel::setData(_index, value, role);

    else if(_index.isValid() && _index.column() == 0 && _index.row() == 0 && role == Qt::CheckStateRole) {
        MainWindow::instance()->setOnlineEnabled(online == Enabled ? false : true);
        return true;
    }

    return QAbstractProxyModel::setData(index(_index.row()-1, _index.column(), _index.parent()), value, role);
}

}}}
