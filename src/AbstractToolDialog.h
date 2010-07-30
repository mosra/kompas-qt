#ifndef Map2X_QtGui_AbstractToolDialog_h
#define Map2X_QtGui_AbstractToolDialog_h
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
 * @brief Class Map2X::QtGui::AbstractToolDialog
 */

#include <QtGui/QDialog>

namespace Map2X { namespace QtGui {

class MainWindow;

/**
 * @brief Abstract tool dialog
 *
 * Subclassed by tool plugins. See also AbstractTool.
 */
class AbstractToolDialog: public QDialog {
    public:
        /**
         * @brief Constructor
         * @param _mainWindow       Pointer to main window
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        inline AbstractToolDialog(MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0): QDialog(parent, f), mainWindow(_mainWindow) {}

    protected:
        MainWindow* mainWindow;     /**< @brief Pointer to MainWindow */
};

}}

#endif
