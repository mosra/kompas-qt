#ifndef Kompas_Plugins_UIComponents_ConfigurationDialog_h
#define Kompas_Plugins_UIComponents_ConfigurationDialog_h
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
 * @brief Class Kompas::Plugins::UIComponents::ConfigurationDialog
 */

#include "AbstractConfigurationDialog.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

/** @brief Main configuration dialog */
class ConfigurationDialog: public QtGui::AbstractConfigurationDialog {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractConfigurationDialog::AbstractConfigurationDialog */
        ConfigurationDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
};


}}}

#endif
