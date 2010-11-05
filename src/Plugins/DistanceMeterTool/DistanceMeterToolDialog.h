#ifndef Map2X_Plugins_DistanceMeterToolDialog_h
#define Map2X_Plugins_DistanceMeterToolDialog_h
/*
    Copyright © 2010 Jan Dupal <dupal.j@seznam.cz>

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
 * @brief Class Map2X::Plugins::DistanceMeterToolDialog
 */

#include "../../AbstractToolDialog.h"

class QDoubleSpinBox;

namespace Map2X {

namespace QtGui {
    class Wgs84CoordsEdit;
}

namespace Plugins {

/** @brief Measuring distance between two WGS84 coords */
class DistanceMeterToolDialog: public QtGui::AbstractToolDialog {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractToolDialog::AbstractToolDialog */
        DistanceMeterToolDialog(QtGui::MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    private slots:
        void calculate();

    private:
        QtGui::Wgs84CoordsEdit *coordsA, *coordsB;
        QDoubleSpinBox* distance;
};

}}

#endif
