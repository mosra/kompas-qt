#ifndef Kompas_Plugins_UIComponents_ConfigurationWidget_h
#define Kompas_Plugins_UIComponents_ConfigurationWidget_h
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
 * @brief Class Kompas::Plugins::UIComponents::ConfigurationWidget
 */

#include "AbstractConfigurationWidget.h"

class QSpinBox;
class QComboBox;
class QLineEdit;

namespace Kompas {

namespace QtGui {
    class PluginModel;
}

namespace Plugins { namespace UIComponents {

/** @brief Widget in main configuration dialog */
class ConfigurationWidget: public QtGui::AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractConfigurationWidget::AbstractConfigurationWidget */
        ConfigurationWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        virtual void reset();
        virtual void restoreDefaults();
        virtual void save();

    private slots:
        void selectPackageDir();

    private:
        QComboBox* mapViewPlugin;
        QtGui::PluginModel* mapViewModel;
        QSpinBox* maxSimultaenousDownloads;
        QLineEdit* packageDir;
};

}}}

#endif
