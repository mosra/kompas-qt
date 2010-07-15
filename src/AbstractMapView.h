#ifndef Map2X_QtGui_AbstractMapView_h
#define Map2X_QtGui_AbstractMapView_h
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
 * @brief Class Map2X::QtGui::AbstractMapView
 */

#include <QtGui/qwidget.h>

#include "PluginManager/definitions.h"
#include "AbstractTileModel.h"

using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

/** @brief Abstract class for map viewer widget plugins */
class AbstractMapView: public QWidget {
    PLUGIN_INTERFACE("cz.mosra.Map2X.QtGui.AbstractMapView/0.1")

    public:
        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param f         Window flags
         */
        inline AbstractMapView(QWidget* parent = 0, Qt::WindowFlags f = 0): QWidget(parent, f), tileModel(0) {}

        /**
         * @brief Set tile model to the view
         * @param model     Tile model
         */
        void setTileModel(AbstractTileModel* model);

        /** @brief Refresh map view */
        virtual void refresh() = 0;

    protected:
        AbstractTileModel* tileModel;   /**< @brief Tile model */
};

}}

#endif
