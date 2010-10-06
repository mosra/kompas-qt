#ifndef Map2X_Plugins_Tile_h
#define Map2X_Plugins_Tile_h
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

#include <QtGui/QGraphicsItemGroup>

#include "AbstractTileModel.h"

namespace Map2X { namespace Plugins {

/** @brief One tile in GraphicsMapView */
class Tile: public QGraphicsItemGroup {
    public:
        /**
         * @brief Constructor
         * @param coords        Tile coordinates
         * @param parent        Parent item
         * @param scene         Scene
         */
        Tile(const Core::TileCoords& coords, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0):
            QGraphicsItemGroup(parent, scene), _coords(coords) {}

        /** @brief Tile coordinates */
        inline Core::TileCoords coords() const { return _coords; }

        /**
         * @brief Set layer
         * @param layer         Layer ID (0 = background layer, everything
         *      another are overlays)
         * @param pixmap        Pixmap
         *
         * Assigns pixmap to given layer.
         */
        void setLayer(int layer, const QPixmap& pixmap);

        /**
         * @brief Set empty layer
         * @param layer         Layer ID (0 = background layer, everything
         *      another are overlays)
         *
         * Creates empty layer as placeholder. If layer with given ID already
         * exists, its pixmap is discarded.
         */
        void setLayer(int layer);

        /**
         * @brief Remove layer
         * @param layer         Layer name
         */
        void removeLayer(int layer);

        /** @brief Layer */
        QGraphicsPixmapItem* layer(int layer);

    private:
        Core::TileCoords _coords;
        QList<QGraphicsPixmapItem*> _layers;
};

}}

#endif
