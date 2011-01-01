#ifndef Kompas_QtGui_MovingWidget_h
#define Kompas_QtGui_MovingWidget_h
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
 * @brief Class Kompas::QtGui::MovingWidget
 */

#include <QtGui/QWidget>

class QPushButton;

namespace Kompas { namespace QtGui {

/**
 * @brief Useless widget for moving the map with frantic mouse clicking
 */
class MovingWidget: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         * @param f             Window flags
         */
        MovingWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

    private slots:
      void move();

    private:
        QPushButton *up,
            *left,
            *right,
            *down;
};

}}

#endif
