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

#include "Tile.h"

namespace Map2X { namespace QtGui {

void Plugins::Tile::setLayer(int layer, const QPixmap& pixmap) {
    if(layer < 0) return;

    setLayer(layer);

    _layers[layer] = new QGraphicsPixmapItem(pixmap, this, scene());
}

void Plugins::Tile::setLayer(int layer) {
    if(layer < 0) return;

    /* If layer number is larger than actual layer count, initialize all missing
        layers (recursively) to invalid pixmaps */
    if(layer > _layers.count()) setLayer(layer-1);

    /* Layer doesn't exist, create null placeholder */
    if(layer == _layers.count()) _layers.append(0);

    /* Layer already exists, discard current data */
    else if(_layers[layer]) {
        delete _layers[layer];
        _layers[layer] = 0;
    }
}

void Plugins::Tile::removeLayer(int layer) {
    if(layer < 0 || layer >= _layers.count()) return;

    delete _layers[layer];
    _layers.removeAt(layer);
}

}}
