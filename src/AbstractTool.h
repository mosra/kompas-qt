#ifndef Map2X_QtGui_AbstractTool_h
#define Map2X_QtGui_AbstractTool_h
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
 * @brief Class Map2X::QtGui::AbstractTool
 */

#include "PluginManager/Plugin.h"

#include <QtCore/QString>
#include <QtGui/QIcon>

#include "AbstractToolDialog.h"

namespace Map2X { namespace QtGui {

/**
 * @brief Abstract tool
 *
 * Base interface for tool plugins.
 */
class AbstractTool: public PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.Map2X.QtGui.AbstractTool/0.1")

    public:
        inline AbstractTool(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""):
            Plugin(manager, plugin) {}

        /** @brief Menu item text */
        virtual QString menuText() const = 0;

        /** @brief Menu item icon */
        virtual QIcon menuIcon() const { return QIcon(); }

        /**
         * @brief Dialog instance
         * @param mainWindow    Pointer to main window
         * @param parent        Parent widget
         * @param f             Window flags
         * @return New instance of tool dialog. Proper object deletion is left
         * on caller.
         */
        virtual AbstractToolDialog* create(MainWindow* mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0) const = 0;
};

}}

#endif
