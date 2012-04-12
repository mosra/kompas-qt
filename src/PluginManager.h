#ifndef Kompas_QtGui_PluginManager_h
#define Kompas_QtGui_PluginManager_h
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
 * @brief Class Kompas::QtGui::PluginManager
 */

#include "PluginManager/PluginManager.h"
#include "AbstractPluginManager.h"
#include "Utility/Resource.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Qt version of Corrade::PluginManager::PluginManager
 *
 * If you want to set parent QObject to the manager, you have to call
 * setParent(), because the original PluginManager doesn't have such
 * constructor.
 *
 * @todo C++11 - do it using using
 */
template<class T> class PluginManager: public Corrade::PluginManager::PluginManager<T, QtGui::AbstractPluginManager> {
    public:
        /** @copydoc PluginManager::PluginManager::PluginManager */
        inline PluginManager(const std::string& pluginDirectory): Corrade::PluginManager::PluginManager<T, QtGui::AbstractPluginManager>(pluginDirectory) {}
};

}}

#endif
