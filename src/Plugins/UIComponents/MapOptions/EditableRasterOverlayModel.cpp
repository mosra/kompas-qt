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

#include "EditableRasterOverlayModel.h"

#include "MainWindow.h"
#include "RasterOverlayModel.h"
#include "AbstractMapView.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

void EditableRasterOverlayModel::setSourceModel(QAbstractItemModel* sourceModel) {
    disconnect(sourceModel, SIGNAL(modelReset()), this, SLOT(reload()));

    QAbstractProxyModel::setSourceModel(sourceModel);
    reload();

    connect(sourceModel, SIGNAL(modelReset()), SLOT(reload()));
}

void EditableRasterOverlayModel::reload() {
    AbstractMapView* mapView = MainWindow::instance()->mapView();

    if(mapView) {
        QStringList loadedOverlays = mapView->overlays();
        reload(loadedOverlays);

    } else {
        beginResetModel();
        loaded.fill(false, sourceModel()->rowCount());
        endResetModel();
    }
}

void EditableRasterOverlayModel::reload(const QStringList& loadedOverlays) {
    beginResetModel();

    loaded.fill(false, sourceModel()->rowCount());
    for(int row = 0; row != sourceModel()->rowCount(); ++row) {
        if(loadedOverlays.contains(sourceModel()->index(row, 0).data().toString()))
            loaded.setBit(row, true);
    }

    endResetModel();
}

QVariant EditableRasterOverlayModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::CheckStateRole && index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount())
        return loaded.at(index.row()) ? Qt::Checked : Qt::Unchecked;

    return QAbstractProxyModel::data(index, role);
}

Qt::ItemFlags EditableRasterOverlayModel::flags(const QModelIndex& index) const {
    if(index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount())
        return QAbstractProxyModel::flags(index)|Qt::ItemIsUserCheckable;

    return QAbstractProxyModel::flags(index);
}

bool EditableRasterOverlayModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    AbstractMapView* mapView = MainWindow::instance()->mapView();

    if(mapView && index.isValid() && index.column() == RasterOverlayModel::Translated && index.row() < rowCount() && role == Qt::CheckStateRole) {
        /* Remove overlay */
        if((loaded.at(index.row()) && mapView->removeOverlay(data(index.sibling(index.row(), RasterOverlayModel::Name)).toString())) ||
          (!loaded.at(index.row()) && mapView->addOverlay(data(index.sibling(index.row(), RasterOverlayModel::Name)).toString())))
            return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

}}}
