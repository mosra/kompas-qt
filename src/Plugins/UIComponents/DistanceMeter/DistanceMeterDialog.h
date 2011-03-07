#ifndef Kompas_Plugins_UIComponents_DistanceMeterDialog_h
#define Kompas_Plugins_UIComponents_DistanceMeterDialog_h
/*
    Copyright © 2010 Jan Dupal <dupal.j@seznam.cz>

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
 * @brief Class Kompas::Plugins::UIComponents::DistanceMeterDialog
 */

#include <QtGui/QDialog>

class QComboBox;
class QDoubleSpinBox;

namespace Kompas {

namespace QtGui {
    class LatLonCoordsEdit;
}

namespace Plugins { namespace UIComponents {

/** @brief Measuring distance between two lat/lon coords */
class DistanceMeterDialog: public QDialog {
    Q_OBJECT

    public:
        DistanceMeterDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

    private slots:
        void calculate();

    private:
        QtGui::LatLonCoordsEdit *coordsA, *coordsB;
        QDoubleSpinBox* distance;
        QComboBox* celestialBody;
};

}}}

#endif
