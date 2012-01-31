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

#include "PluginManagerStore.h"

#include "PluginModel.h"

using namespace std;
using namespace Corrade::Utility;
using namespace Kompas::Core;

namespace Kompas { namespace QtGui {

PluginManagerStore::PluginManagerStore(ConfigurationGroup* configurationGroup, QObject* parent): QObject(parent) {
    _caches = new Item<AbstractCache>(
        configurationGroup->group("caches"),
        tr("Caches"),
        tr("Caching of downloaded data."),
        this);
    _items << _caches;
    _celestialBodies = new Item<AbstractCelestialBody>(
        configurationGroup->group("celestialBodies"),
        tr("Celestial bodies"),
        tr("Planets, moons, stars and other astronomical objects."),
        this);
    _items << _celestialBodies;
    _mapViews = new Item<AbstractMapView>(
        configurationGroup->group("mapViews"),
        tr("Map viewers"),
        tr("Interactively presenting the map to you."),
        this);
    _items << _mapViews;
    _projections = new Item<AbstractProjection>(
        configurationGroup->group("projections"),
        tr("Projections"),
        tr("Providing coordinate conversion for particular maps."),
        this);
    _items << _projections;
    _rasterModels = new Item<AbstractRasterModel>(
        configurationGroup->group("rasterModels"),
        tr("Raster maps"),
        tr("Allow opening different formats of raster maps."),
        this);
    _items << _rasterModels;
    _uiComponents = new Item<AbstractUIComponent>(
        configurationGroup->group("uiComponents"),
        tr("User interface components"),
        tr("Parts of user interface."),
        this);
    _items << _uiComponents;

    /* Load all plugins as configured */
    foreach(AbstractItem* item, _items)
        item->loadedFromConfiguration();
}

PluginManagerStore::AbstractItem::AbstractItem(Corrade::Utility::ConfigurationGroup* configurationGroup, const QString& name, const QString& description, AbstractPluginManager* manager, QObject* parent): QObject(parent), _configurationGroup(configurationGroup), _name(name), _description(description), _manager(manager) {
    _model = new PluginModel(_manager, 0, this);
    /** @todo Make this proxy to original model */
    _loadedOnlyModel = new PluginModel(_manager, PluginModel::LoadedOnly, this);
}

void PluginManagerStore::AbstractItem::pluginDirectoryFromConfiguration() {
    _manager->setPluginDirectory(_configurationGroup->value<string>("__dir"));
}

void PluginManagerStore::AbstractItem::pluginDirectoryToConfiguration() {
    _configurationGroup->setValue<string>("__dir", _manager->pluginDirectory());
}

void PluginManagerStore::AbstractItem::loadedFromConfiguration() {
    vector<string> plugins = _manager->pluginList();

    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
        if(_configurationGroup->value<bool>(*it))
            _manager->load(*it);
}

void PluginManagerStore::AbstractItem::loadedToConfiguration() {
    vector<string> plugins = _manager->pluginList();

    for(vector<string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
        if(_manager->loadState(*it) & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))
            _configurationGroup->setValue<bool>(*it, true);
        else
            _configurationGroup->removeValue(*it);
}

}}
