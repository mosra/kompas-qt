#ifndef Kompas_QtGui_AbstractConfigurationDialog_h
#define Kompas_QtGui_AbstractConfigurationDialog_h
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
 * @brief Class Kompas::QtGui::AbstractConfigurationDialog
 */

#include <QtGui/QDialog>

#include "AbstractConfigurationWidget.h"

class QVBoxLayout;
class QPushButton;
class QDialogButtonBox;

namespace Kompas { namespace QtGui {

class MainWindow;

/**
 * @brief Abstract configuration dialog
 *
 * Provides save, reset and restoreDefaults buttons which are connected to one
 * or more subclasses of AbstractConfigurationWidget.
 */
class AbstractConfigurationDialog: public QDialog {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         * @param f             Window flags
         */
        AbstractConfigurationDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

        /**
         * @brief Set main widget of the dialog
         * @param widget        Widget
         *
         * @attention Only one of setWidget() and setLayout() functions should
         * be called.
         */
        void setCentralWidget(QWidget* widget);

        /**
         * @brief Set main layout of the dialog
         * @param layout        Layout
         *
         * @attention Only one of setWidget() and setLayout() functions should
         * be called.
         */
        void setCentralLayout(QLayout* layout);

        /**
         * @brief Connect configuration widget to this dialog
         * @param widget        Configuration widget
         *
         * Enables reseting and saving configuration in given widget.
         */
        void connectWidget(AbstractConfigurationWidget* widget);

    public slots:
        /**
         * @brief Require application restart
         *
         * Sets required restart. After saving the dialog an messagebox is
         * shown, which recommends restarting application.
         */
        inline void requireRestart(bool require = true) { restartRequired = require; }

    signals:
        /**
         * @brief Restore default values
         *
         * This signal is emitted after clicking on Restore Defaults button and
         * accepting the message box. All widgets connected with connectWidget()
         * recieve it automatically.
         */
        void restoreDefaults(bool = true);

    private slots:
        void restoreDefaultsWarning();
        void restartRequiredWarning();

    private:
        QDialogButtonBox* buttons;
        QPushButton *restoreDefaultsButton,
            *resetButton,
            *saveButton,
            *cancelButton;
        QVBoxLayout* _layout;

        bool restartRequired;
};

}}

#endif
