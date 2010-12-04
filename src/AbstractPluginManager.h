#ifndef Kompas_QtGui_AbstractPluginManager_h
#define Kompas_QtGui_AbstractPluginManager_h
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
 * @brief Class Kompas::QtGui::AbstractPluginManager
 */

#include <QtCore/QObject>

#include "PluginManager/AbstractPluginManager.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Qt version of PluginManager::AbstractPluginManager
 *
 * Instead of PluginManager::AbstractPluginManager provides signal notification
 * when a plugin is loaded or unloaded.
 */
class AbstractPluginManager: public QObject, public Kompas::PluginManager::AbstractPluginManager {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param pluginDirectory       Plugin directory
         * @param parent                Parent object
         * @copydetails PluginManager::AbstractPluginManager::AbstractPluginManager()
         */
        inline AbstractPluginManager(const std::string& pluginDirectory, QObject* parent = 0): QObject(parent), Kompas::PluginManager::AbstractPluginManager(pluginDirectory) {}

        /**
         * @copydoc PluginManager::AbstractPluginManager::load()
         * Emits loadAttempt().
         */
        inline LoadState load(const std::string& name) {
            LoadState before = loadState(name);
            LoadState after = Kompas::PluginManager::AbstractPluginManager::load(name);
            emit loadAttempt(name, before, after);
            return after;
        }

        /**
         * @copydoc PluginManager::AbstractPluginManager::unload()
         * Emits unloadAttempt().
         */
        inline LoadState unload(const std::string& name) {
            LoadState before = loadState(name);
            LoadState after = Kompas::PluginManager::AbstractPluginManager::unload(name);
            emit unloadAttempt(name, before, after);
            return after;
        }

    signals:
        /**
         * @brief Plugin load attempt
         * @param name      Plugin name
         * @param before    State before load attempt
         * @param after     State after load attempt
         */
        void loadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);

        /**
         * @brief Plugin unload attempt
         * @param name      Plugin name
         * @param before    State before unload attempt
         * @param after     State after unload attempt
         */
        void unloadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);
};

}}

#endif
