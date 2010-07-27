#ifndef Map2X_QtGui_PluginDialog_h
#define Map2X_QtGui_PluginDialog_h
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
 * @brief Class Map2X::QtGui::PluginDialog
 */

#include <QtGui/QDialog>

class QDialogButtonBox;
class QTabWidget;

namespace Map2X { namespace QtGui {

class MainWindow;

/**
 * @brief Plugin settings dialog
 *
 * Provides viewing and managing plugins.
 * @todo Settings for plugin dir, autoloading of all, ...
 */
class PluginDialog: public QDialog {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param mainWindow        Pointer to main window
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        PluginDialog(MainWindow* mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    private slots:
        void restoreDefaultsWarning();

    signals:
        void restoreDefaults();

    private:
        QTabWidget* tabs;
        QDialogButtonBox* buttons;
        QPushButton *restoreDefaultsButton,
            *resetButton;
};

}}

#endif
