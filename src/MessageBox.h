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

#ifdef _WIN32
    #undef MessageBox   /* I HATE you, Windoze */
#endif

namespace Kompas { namespace QtGui {

/**
 * @brief Message box
 *
 * Replaces system-specific icons with custom.
 */
class MessageBox: public QMessageBox {
    public:
        /** @brief Information messagebox */
        static QMessageBox::StandardButton information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = Ok, QMessageBox::StandardButton defaultButton = NoButton);

        /** @brief Question messagebox */
        static QMessageBox::StandardButton question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = Ok, QMessageBox::StandardButton defaultButton = NoButton);

        /** @brief Warning messagebox */
        static QMessageBox::StandardButton warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = Ok, QMessageBox::StandardButton defaultButton = NoButton);

        /** @brief Critical messagebox */
        static QMessageBox::StandardButton critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = Ok, QMessageBox::StandardButton defaultButton = NoButton);

        /** @brief Constructor */
        inline MessageBox(QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = NoButton, QWidget* parent = 0, Qt::WindowFlags flags = Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint): QMessageBox(icon, title, text, buttons, parent, flags) { setIcon(icon); }

        /**
         * @brief Default constructor
         * @param   parent      Parent widget
         */
        inline MessageBox(QWidget* parent = 0): QMessageBox(parent) {}

        /**
         * @brief Set custom icon
         * @param   icon        Icon
         */
        void setIcon(QMessageBox::Icon icon);
};

}}

#endif
