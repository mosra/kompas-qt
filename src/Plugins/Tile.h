#ifndef Map2X_QtGui_Plugins_Tile_h
#define Map2X_QtGui_Plugins_Tile_h
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
 * @brief Class Map2X::QtGui::Plugins::Tile
 */

#include <QtGui/QGraphicsPixmapItem>

#include "AbstractTileModel.h"

namespace Map2X { namespace QtGui { namespace Plugins {

/** @brief One tile in GraphicsMapView */
class Tile: public QGraphicsPixmapItem {
    public:
        /**
         * @brief Constructor
         * @param coords        Tile coordinates
         * @param pixmap        Tile data
         * @param parent        Parent item
         * @param scene         Scene
         */
        Tile(const Core::TileCoords& coords, const QPixmap& pixmap, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0):
            QGraphicsPixmapItem(pixmap, parent, scene), _coords(coords) {}

        /** @brief Tile coordinates */
        inline Core::TileCoords coords() const { return _coords; }

    private:
        Core::TileCoords _coords;
};

}}}

#endif
