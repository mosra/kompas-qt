#ifndef Map2X_QtGui_OpenRasterMenuView_h
#define Map2X_QtGui_OpenRasterMenuView_h
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

/** @file
 * @brief Class Map2X::QtGui::OpenRasterMenuView
 */

#include "AbstractPluginMenuView.h"
#include "PluginManager.h"
#include "AbstractRasterModel.h"

namespace Map2X { namespace QtGui {

/**
 * @brief Menu view for Open Raster menu
 *
 * Displays only these plugins which have
 * Core::AbstractRasterModel::LoadableFromUrl feature.
 */
class OpenRasterMenuView: public AbstractPluginMenuView {
    public:
        /** @copydoc QtGui::AbstractPluginMenuView */
        OpenRasterMenuView(PluginManager<Core::AbstractRasterModel>* manager, QMenu* menu, QAction* before = 0, QObject* parent = 0):
            AbstractPluginMenuView(manager, menu, before, parent), rasterManager(manager) {}

    private slots:
        virtual void trigger(QAction* action);

    private:
        PluginManager<Core::AbstractRasterModel>* rasterManager;
        QHash<QAction*, QString> items;

        virtual QAction* createMenuAction(const std::string& pluginName);
};

}}

#endif