#ifndef Kompas_QtGui_LatLonCoordsEdit_h
#define Kompas_QtGui_LatLonCoordsEdit_h
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
 * @brief Class Kompas::QtGui::LatLonCoordsEdit
 */

#include <QtGui/QLineEdit>

#include "LatLonCoords.h"

namespace Kompas { namespace QtGui {

/** @brief Edit box for LatLonCoords */
class LatLonCoordsEdit: public QLineEdit {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         */
        inline LatLonCoordsEdit(QWidget* parent = 0):
            QLineEdit(parent) { init(); }

        /**
         * @brief Constructor
         * @param coords        Coordinates as string
         * @param parent        Parent widget
         *
         * If the string cannot be parsed, no coordinates are set.
         */
        inline LatLonCoordsEdit(const QString& coords, QWidget* parent = 0):
            QLineEdit(parent) { init(); setText(coords); }

        /**
         * @brief Constructor
         * @param coords        Coordinates
         * @param parent        Parent widget
         */
        inline LatLonCoordsEdit(const Core::LatLonCoords& coords, QWidget* parent = 0):
            QLineEdit(parent) { init(); setCoords(coords); }

        /** @brief Coordinates */
        inline Core::LatLonCoords coords() const {
            return Core::LatLonCoords(text().toStdString());
        }

        /**
         * @brief Set coordinates
         * @param coords        Coordinates
         */
        inline void setCoords(const Core::LatLonCoords& coords) {
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
