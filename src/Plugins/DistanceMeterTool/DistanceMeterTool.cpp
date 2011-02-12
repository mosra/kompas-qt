#ifndef Kompas_Plugins_DistanceMeterTool_h
#define Kompas_Plugins_DistanceMeterTool_h
/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2010 Jan Dupal <dupal.j@seznam.cz>

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
 * @brief Class Kompas::Plugins::DistanceMeterTool
 */

#include "AbstractTool.h"
#include "DistanceMeterToolDialog.h"

namespace Kompas { namespace Plugins {

/** @brief Measuring distance between two lat/lon coords */
class DistanceMeterTool: public QtGui::AbstractTool {
    public:
        /** @copydoc QtGui::AbstractTool::AbstractTool */
        DistanceMeterTool(Kompas::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""):
            AbstractTool(manager, plugin) {}

        virtual QtGui::AbstractToolDialog* create(QWidget* parent = 0, Qt::WindowFlags f = 0) const
            { return new DistanceMeterToolDialog(this, parent, f); }
};

}}

PLUGIN_REGISTER_STATIC(DistanceMeterTool,
                       Kompas::Plugins::DistanceMeterTool,
                       "cz.mosra.Kompas.QtGui.AbstractTool/0.2")

#endif
