#ifndef Map2X_QtGui_PluginManager_h
#define Map2X_QtGui_PluginManager_h
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
 * @brief Class Map2X::QtGui::PluginManager
 */

#include "AbstractPluginManager.h"
#include "PluginManager/Plugin.h"
#include "Utility/Resource.h"

namespace Map2X { namespace QtGui {

/* The same code as in Map2X::PluginManager::PluginManager */
#define MAP2X_SKIP_PLUGINMANAGER_NAMESPACE
#include "PluginManager/PluginManager.h"

/* Only for doxygen documentation */
#ifdef DOXYGEN_GENERATING_OUTPUT
/**
 * @brief Qt version of PluginManager::PluginManager
 * @copydetails Map2X::PluginManager::PluginManager
 *
 * Exactly the same code as PluginManager::PluginManager, but this class
 * depends on QtGui::AbstractPluginManager instead of
 * PluginManager::AbstractPluginManager.
 */
template<class T> class PluginManager: public AbstractPluginManager {
    public:
        /** @copydoc PluginManager::PluginManager::PluginManager */
        PluginManager(const std::string& pluginDirectory);

        /** @copydoc PluginManager::PluginManager::pluginInterface() */
        std::string pluginInterface();

        /** @copydoc PluginManager::PluginManager::instance() */
        T* instance(const std::string& name);

        /** @copydoc PluginManager::PluginManager::nameOfInstance() */
        std::string nameOfInstance(const T* instance);
};
#endif

}}

#endif
