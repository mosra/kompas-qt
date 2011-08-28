#ifndef Kompas_QtGui_AbstractPluginManager_h
#define Kompas_QtGui_AbstractPluginManager_h
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
 * @brief Class Kompas::QtGui::AbstractPluginManager
 */

#include <QtCore/QObject>

#include "PluginManager/AbstractPluginManager.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Qt version of PluginManager::AbstractPluginManager
 *
 * Instead of PluginManager::AbstractPluginManager provides signal notification
 * when a plugin is loaded, unloaded or reloaded.
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
        inline LoadState load(const std::string& _plugin) {
            LoadState before = loadState(_plugin);
            LoadState after = Kompas::PluginManager::AbstractPluginManager::load(_plugin);
            emit loadAttempt(_plugin, before, after);
            return after;
        }

        /**
         * @copydoc PluginManager::AbstractPluginManager::unload()
         * Emits unloadAttempt().
         */
        inline LoadState unload(const std::string& _plugin) {
            LoadState before = loadState(_plugin);
            LoadState after = Kompas::PluginManager::AbstractPluginManager::unload(_plugin);
            emit unloadAttempt(_plugin, before, after);
            return after;
        }

    public slots:
        /**
         * @copydoc PluginManager::AbstractPluginManager::reloadPluginDirectory()
         * Emits pluginDirectoryReloaded().
         */
        inline void reloadPluginDirectory() {
            Kompas::PluginManager::AbstractPluginManager::reloadPluginDirectory();
            emit pluginDirectoryReloaded();
        }

    protected:
        /**
         * @copydoc PluginManager::AbstractPluginManager::reloadPluginMetadata()
         * If the plugin disappears, emits pluginDisappeared().
         */
        bool reloadPluginMetadata(std::map<std::string, PluginObject*>::iterator it) {
            if(!Kompas::PluginManager::AbstractPluginManager::reloadPluginMetadata(it)) {
                emit pluginDisappeared(it->first);
                return false;
            }
            emit pluginMetadataReloaded(it->first);
            return true;
        }

    signals:
        /**
         * @brief Plugin load attempt
         * @param plugin    Plugin
         * @param before    State before load attempt
         * @param after     State after load attempt
         *
         * @note Before plugin load the metadata are fully reloaded. Also this
         * signal can be emitted after the plugin disappeared, in that case
         * plugin with this name doesn't exist anymore.
         */
        void loadAttempt(const std::string& plugin, int before, int after);

        /**
         * @brief Plugin unload attempt
         * @param plugin    Plugin
         * @param before    State before unload attempt
         * @param after     State after unload attempt
         *
         * @note After plugin unload the metadata are fully reloaded. Also this
         * signal can be emitted after the plugin disappeared, in that case
         * plugin with this name doesn't exist anymore.
         */
        void unloadAttempt(const std::string& plugin, int before, int after);

        /**
         * @brief Plugin directory was reloaded
         */
        void pluginDirectoryReloaded();

        /**
         * @brief Plugin was reloaded
         */
        void pluginMetadataReloaded(const std::string& plugin);

        /**
         * @brief Plugin disappeared
         *
         * Emitted when plugin binary is not found on plugin reload attempt.
         */
        void pluginDisappeared(const std::string& plugin);
};

}}

#endif
