#ifndef Kompas_QtGui_Wgs84CoordsEdit_h
#define Kompas_QtGui_Wgs84CoordsEdit_h
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

/** @file
 * @brief Class Kompas::QtGui::Wgs84CoordsEdit
 */

#include <QtGui/QLineEdit>

#include "Wgs84Coords.h"

namespace Kompas { namespace QtGui {

/** @brief Edit box for Wgs84Coords */
class Wgs84CoordsEdit: public QLineEdit {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         */
        inline Wgs84CoordsEdit(QWidget* parent = 0):
            QLineEdit(parent) { init(); }

        /**
         * @brief Constructor
         * @param coords        Coordinates as string
         * @param parent        Parent widget
         *
         * If the string cannot be parsed, no coordinates are set.
         */
        inline Wgs84CoordsEdit(const QString& coords, QWidget* parent = 0):
            QLineEdit(parent) { init(); setText(coords); }

        /**
         * @brief Constructor
         * @param coords        Coordinates
         * @param parent        Parent widget
         */
        inline Wgs84CoordsEdit(const Core::Wgs84Coords& coords, QWidget* parent = 0):
            QLineEdit(parent) { init(); setCoords(coords); }

        /** @brief Coordinates */
        inline Core::Wgs84Coords coords() const {
            return Core::Wgs84Coords(text().toStdString());
        }

        /**
         * @brief Set coordinates
         * @param coords        Coordinates
         */
        inline void setCoords(const Core::Wgs84Coords& coords) {
            setText(QString::fromStdString(coords.toString()));
        }

    private slots:
        void checkValidity();
        void convert();

    private:
        void init();

        QColor defaultColor;
};

}}

#endif
