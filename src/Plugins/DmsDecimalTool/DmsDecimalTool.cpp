#ifndef Kompas_Plugins_DmsDecimalTool_h
#define Kompas_Plugins_DmsDecimalTool_h
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
 * @brief Class Kompas::Plugins::DmsDecimalTool
 */

#include "AbstractTool.h"
#include "DmsDecimalToolDialog.h"

namespace Kompas { namespace Plugins {

/** @brief Converting DMS to decimal and back */
class DmsDecimalTool: public QtGui::AbstractTool {
    public:
        /** @copydoc QtGui::AbstractTool::AbstractTool */
        DmsDecimalTool(Kompas::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""):
            AbstractTool(manager, plugin) {}

        virtual QtGui::AbstractToolDialog* create(QWidget* parent = 0, Qt::WindowFlags f = 0) const
            { return new DmsDecimalToolDialog(this, parent, f); }
};

}}

PLUGIN_REGISTER_STATIC(DmsDecimalTool, Kompas::Plugins::DmsDecimalTool,
                       "cz.mosra.Kompas.QtGui.AbstractTool/0.1")

#endif
