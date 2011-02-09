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

#include "CurrentCelestialBodyPluginModel.h"

#include "MainWindow.h"
#include "PluginModel.h"

using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins {

CurrentCelestialBodyPluginModel::CurrentCelestialBodyPluginModel(QObject* parent): QAbstractProxyModel(parent) {
    connect(MainWindow::instance(), SIGNAL(rasterModelChanged(const Core::AbstractRasterModel*)), SLOT(changeCurrent(const Core::AbstractRasterModel*)));
}

void CurrentCelestialBodyPluginModel::setSourceModel(PluginModel* sourceModel) {
    QAbstractProxyModel::setSourceModel(sourceModel);

    pluginModel = sourceModel;

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    connect(sourceModel, SIGNAL(modelAboutToBeReset()), SIGNAL(modelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()), SIGNAL(modelReset()));
    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(rowsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(rowsRemoved(QModelIndex,int,int)));
}

QVariant CurrentCelestialBodyPluginModel::data(const QModelIndex& index, int role) const {
    /* Mark current celestial body */
    if(role == Qt::DisplayRole && index.column() == PluginModel::Name) {
        const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
        bool isCurrent = rasterModel && pluginModel->index(index.row(), PluginModel::Plugin).data().toString() == QString::fromStdString(rasterModel->celestialBody());
        MainWindow::instance()->unlockRasterModel();

        if(isCurrent)
            return tr("Current (%0)").arg(sourceModel()->data(index, role).toString());
    }

    return pluginModel->data(index, role);
}

void CurrentCelestialBodyPluginModel::changeCurrent(const AbstractRasterModel* previous) {
    /* Remove 'current' mark from previous */
    if(previous != 0) {
        int row = pluginModel->findPlugin(QString::fromStdString(previous->celestialBody()));
        if(row != -1) emit dataChanged(index(row, PluginModel::Name), index(row, PluginModel::Name));
    }

    int row = -1;
    const AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForRead();
    if(rasterModel) row = pluginModel->findPlugin(QString::fromStdString(rasterModel->celestialBody()));
    MainWindow::instance()->unlockRasterModel();

    if(row != -1) emit dataChanged(index(row, PluginModel::Name), index(row, PluginModel::Name));
}

}}
