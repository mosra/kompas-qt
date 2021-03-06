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

#include "LatLonCoordsEdit.h"
#include <QtGui/QRegExpValidator>

using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

void LatLonCoordsEdit::init() {
    /* After finished editing try to convert the text to LatLonCoords */
    connect(this, SIGNAL(textChanged(QString)), SLOT(checkValidity()));
    connect(this, SIGNAL(editingFinished()), SLOT(convert()));

    /* Save default box color */
    defaultColor = palette().color(QPalette::Base);

    /** @todo Localized NWES? */
    setText(QString::fromStdString(LatLonCoords().toString(3, true)));
}

void LatLonCoordsEdit::checkValidity() {
    QPalette p = palette();

    LatLonCoords c(text().toStdString());

    if(!c.isValid())
        p.setColor(QPalette::Base, QColor("#ffcccc"));
    else if(QString::fromStdString(c.toString(3, true)) != text())
        p.setColor(QPalette::Base, QColor("#ffffcc"));
    else
        p.setColor(QPalette::Base, defaultColor);

    setPalette(p);
}

void LatLonCoordsEdit::convert() {
    LatLonCoords c(text().toStdString());

    if(c.isValid()) setText(QString::fromStdString(c.toString(3, true)));
}

}}
