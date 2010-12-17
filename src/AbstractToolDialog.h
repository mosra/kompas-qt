#ifndef Kompas_QtGui_AbstractToolDialog_h
#define Kompas_QtGui_AbstractToolDialog_h
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
 * @brief Class Kompas::QtGui::AbstractToolDialog
 */

#include <QtGui/QDialog>

#include "AbstractTool.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Abstract tool dialog
 *
 * Subclassed by tool plugins. See also AbstractTool.
 */
class AbstractToolDialog: public QDialog {
    public:
        /**
         * @brief Constructor
         * @param _tool             Pointer to tool which created this window
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        inline AbstractToolDialog(const AbstractTool* _tool, QWidget* parent = 0, Qt::WindowFlags f = 0): QDialog(parent, f), tool(_tool) {
            setAttribute(Qt::WA_DeleteOnClose);
            if(tool && tool->metadata()) setWindowTitle(QString::fromStdString(*tool->metadata()->name()));
        }

    protected:
        const AbstractTool* tool;     /**< @brief Pointer to tool which created this window */
};

}}

#endif
