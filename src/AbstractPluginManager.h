#ifndef Map2X_QtGui_AbstractPluginManager_h
#define Map2X_QtGui_AbstractPluginManager_h
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

#include <QtCore/QObject>

#include "PluginManager/AbstractPluginManager.h"

namespace Map2X { namespace QtGui {

class AbstractPluginManager: public QObject, public Map2X::PluginManager::AbstractPluginManager {
    Q_OBJECT

    public:
        inline AbstractPluginManager(const std::string& pluginDirectory, QObject* parent = 0): QObject(parent), Map2X::PluginManager::AbstractPluginManager(pluginDirectory) {}

        inline LoadState load(const std::string& name) {
            LoadState before = loadState(name);
            LoadState after = Map2X::PluginManager::AbstractPluginManager::load(name);
            emit loadAttempt(name, before, after);
            return after;
        }

        inline LoadState unload(const std::string& name) {
            LoadState before = loadState(name);
            LoadState after = Map2X::PluginManager::AbstractPluginManager::unload(name);
            emit unloadAttempt(name, before, after);
            return after;
        }

    signals:
        void loadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);
        void unloadAttempt(const std::string& name, AbstractPluginManager::LoadState state, AbstractPluginManager::LoadState after);
};

}}

#endif
