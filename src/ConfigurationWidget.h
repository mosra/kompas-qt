#ifndef Map2X_QtGui_ConfigurationWidget_h
#define Map2X_QtGui_ConfigurationWidget_h
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
 * @brief Class Map2X::QtGui::ConfigurationWidget
 */

#include "AbstractConfigurationWidget.h"

class QCheckBox;
class QSpinBox;
class QComboBox;

namespace Map2X { namespace QtGui {

class Wgs84CoordsEdit;
class MainWindow;

/** @brief Widget in main configuration dialog */
class ConfigurationWidget: public AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _mainWindow       Pointer to main window
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        ConfigurationWidget(MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        virtual void reset();
        virtual void restoreDefaults();
        virtual void save();

    private:
        MainWindow* mainWindow;
        QComboBox* mapViewPlugin;
        Wgs84CoordsEdit* homePosition;
        QCheckBox* enableOnlineMaps;
        QSpinBox* maxSimultaenousDownloads;
};

}}

#endif
