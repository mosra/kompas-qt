#ifndef Kompas_QtGui_MessageBox_h
#define Kompas_QtGui_MessageBox_h
/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Kompas::QtGui::MessageBox
 */

#include <QtGui/QMessageBox>

namespace Kompas { namespace QtGui {

/**
 * @brief Message box
 *
 * Replaces system-specific icons with custom.
 */
class MessageBox: public QMessageBox {
    public:
        static StandardButton information(QWidget* parent, const QString& title, const QString& text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
        static StandardButton question(QWidget* parent, const QString& title, const QString& text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
        static StandardButton warning(QWidget* parent, const QString& title, const QString& text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
        static StandardButton critical(QWidget* parent, const QString& title, const QString& text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);

        inline MessageBox(Icon icon, const QString& title, const QString& text, StandardButtons buttons = NoButton, QWidget* parent = 0, Qt::WindowFlags flags = Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint): QMessageBox(icon, title, text, buttons, parent, flags) { setIcon(icon); }

        inline MessageBox(QWidget* parent = 0): QMessageBox(parent) {}

        void setIcon(Icon icon);
};

}}

#endif
