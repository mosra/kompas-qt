#ifndef Kompas_QtGui_AbstractTool_h
#define Kompas_QtGui_AbstractTool_h
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
 * @brief Class Kompas::QtGui::AbstractTool
 */

#include "PluginManager/Plugin.h"

#include <QtCore/QString>
#include <QtGui/QIcon>

namespace Kompas { namespace QtGui {

class AbstractToolDialog;

/**
 * @brief Abstract tool
 *
 * Base interface for tool plugins.
 */
class AbstractTool: public Kompas::PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.Kompas.QtGui.AbstractTool/0.1")

    public:
        /** @copydoc PluginManager::Plugin::Plugin */
        inline AbstractTool(Kompas::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""):
            Plugin(manager, plugin) {}

        /** @brief Menu item text */
        QString menuText() const {
            if(metadata()) return QString::fromStdString(*metadata()->name());
            else return "";
        }

        /** @brief Menu item icon */
        virtual QIcon menuIcon() const { return QIcon(); }

        /**
         * @brief Dialog instance
         * @param parent        Parent widget
         * @param f             Window flags
         * @return New instance of tool dialog. Proper object deletion is left
         * on caller.
         */
        virtual AbstractToolDialog* create(QWidget* parent = 0, Qt::WindowFlags f = 0) const = 0;
};

}}

#endif
