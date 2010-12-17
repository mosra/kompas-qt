#ifndef Kompas_QtGui_ConfigurationDialog_h
#define Kompas_QtGui_ConfigurationDialog_h
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
 * @brief Class Kompas::QtGui::ConfigurationDialog
 */

#include "AbstractConfigurationDialog.h"
#include "AbstractConfigurationWidget.h"

class QSpinBox;
class QComboBox;
class QLineEdit;

namespace Kompas { namespace QtGui {

/** @brief Main configuration dialog */
class ConfigurationDialog: public AbstractConfigurationDialog {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param mainWindow        Pointer to main window
         * @param f                 Flags
         */
        ConfigurationDialog(MainWindow* mainWindow, Qt::WindowFlags f = 0);

    protected:
        class Widget;
};

/** @brief Widget in main configuration dialog */
class ConfigurationDialog::Widget: public AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        Widget(QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        virtual void reset();
        virtual void restoreDefaults();
        virtual void save();

    private slots:
        void selectPackageDir();

    private:
        QComboBox* mapViewPlugin;
        QSpinBox* maxSimultaenousDownloads;
        QLineEdit* packageDir;
};

}}

#endif
