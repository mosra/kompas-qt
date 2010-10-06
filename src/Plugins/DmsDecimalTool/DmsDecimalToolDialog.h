#ifndef Map2X_Plugins_DmsDecimalToolDialog_h
#define Map2X_Plugins_DmsDecimalToolDialog_h
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
 * @brief Class Map2X::QtGui::Plugins::DmsDecimalToolDialog
 */

#include "../../AbstractToolDialog.h"

class QDoubleSpinBox;

namespace Map2X {

namespace QtGui {
    class Wgs84CoordsEdit;
}

namespace Plugins {

/** @brief Converting DMS to decimal and back */
class DmsDecimalToolDialog: public QtGui::AbstractToolDialog {
    Q_OBJECT

    public:
        DmsDecimalToolDialog(QtGui::MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    private slots:
        void toDecimal();
        void toDms();

    private:
        QtGui::Wgs84CoordsEdit* coords;
        QDoubleSpinBox *latitude,
            *longtitude;
};

}}

#endif
