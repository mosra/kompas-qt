#ifndef Kompas_QtGui_PluginManagerStore_h
#define Kompas_QtGui_PluginManagerStore_h
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
 * @brief Class Kompas::QtGui::PluginManagerStore
 */

#include "PluginManager.h"

#include "AbstractCache.h"
#include "AbstractCelestialBody.h"
#include "AbstractMapView.h"
#include "AbstractProjection.h"
#include "AbstractUIComponent.h"

namespace Kompas { namespace QtGui {

class PluginModel;

/**
@brief Store for different plugin managers

Centralized class which contains all plugin managers along with data access
models. Each store item can be accessed via specific member function such
as @ref rasterModels(), all items together can be accessed via @ref items().

@configuration

<p>All plugin configuration is stored in configuration group given to the
constructor.</p>
<p>There are several plugin groups, each group has its own configuration group
which contains parameter <tt>__dir</tt>, which points to plugin directory for
given plugin group, and several parameters named after plugins with boolean
values indicating whether the plugin is loaded or not. Load state of static
plugins is saved too (set to <tt>true</tt>), so when any plugin is made
non-static in next version, it will be still loaded.</p>
<p>Example structure with all current plugin group names. Group names correspond
with plugin manangers' accessor function names.</p>
<pre>
[caches]
__dir=
pluginName1=
pluginName2=

# ...
[celestialBodies]
[mapViews]
[projections]
[rasterModels]
[uiComponents]
</pre>
*/
class PluginManagerStore: public QObject {
    Q_OBJECT

    public:
        class AbstractItem;
        template<class Interface> class Item;

        /**
         * @brief Constructor
         * @param configurationGroup    Group with configuration of all plugins
         * @param parent                Parent object
         */
        PluginManagerStore(Utility::ConfigurationGroup* configurationGroup, QObject* parent = 0);

        /**
         * @brief All store items
         *
         * List of all store (abstract) items, templated (non-abstract) items
         * are also accessible via specific member functions.
         */
        inline const QList<AbstractItem*>& items() const { return _items; }

        /** @brief Cache plugin manager */
        inline Item<Core::AbstractCache>* caches()
            { return _caches; }

        /** @brief Celestial bodies plugin manager */
        inline Item<Core::AbstractCelestialBody>* celestialBodies()
            { return _celestialBodies; }

        /** @brief Map views plugin manager */
        inline Item<AbstractMapView>* mapViews()
            { return _mapViews; }

        /** @brief Projections plugin manager */
        inline Item<Core::AbstractProjection>* projections()
            { return _projections; }

        /** @brief Raster models plugin manager */
        inline Item<Core::AbstractRasterModel>* rasterModels()
            { return _rasterModels; }

        /** @brief UI components plugin manager */
        inline Item<AbstractUIComponent>* uiComponents()
            { return _uiComponents; }

    private:
        Item<AbstractMapView>* _mapViews;
        Item<Core::AbstractCache>* _caches;
        Item<Core::AbstractCelestialBody>* _celestialBodies;
        Item<Core::AbstractProjection>* _projections;
        Item<Core::AbstractRasterModel>* _rasterModels;
        Item<AbstractUIComponent>* _uiComponents;

        QList<AbstractItem*> _items;
};

/**
 * @brief Plugin manager store abstract item
 *
 * Contains (abstract) plugin manager instance, name and description for
 * particular plugin type and models for data access. See also
 * @ref PluginManagerStore::Item, which provides access to templated
 * (non-abstract) plugin manager.
 */
class PluginManagerStore::AbstractItem: public QObject {
    public:
        /**
         * @brief Constructor
         *
         * Items are accessible via @ref PluginManagerStore::items(), there is
         * no need to construct them manually.
         */
        AbstractItem(Utility::ConfigurationGroup* configurationGroup, const QString& name, const QString& description, AbstractPluginManager* manager, QObject* parent = 0);

        /**
         * @brief Destructor
         *
         * Deletes associated plugin manager.
         */
        virtual ~AbstractItem() { delete _manager; }

        /** @brief Name of plugin type */
        inline QString name() const { return _name; }

        /** @brief Description of plugin type */
        inline QString description() const { return _description; }

        /** @brief Configuration group for this particular plugin type */
        inline Utility::ConfigurationGroup* configurationGroup() { return _configurationGroup; }

        /** @brief Abstract plugin manager */
        inline AbstractPluginManager* manager() { return _manager; }

        /** @brief Plugin model */
        inline PluginModel* model() { return _model; }

        /** @brief Plugin model displaying only loaded plugins */
        inline PluginModel* loadedOnlyModel() { return _loadedOnlyModel; }

        /** @brief Set plugin directory from configuration */
        void pluginDirectoryFromConfiguration();

        /** @brief Save plugin directory to configuration */
        void pluginDirectoryToConfiguration();

        /** @brief Load plugins as configured */
        void loadedFromConfiguration();

        /** @brief Save list of loaded plugins to configuration */
        void loadedToConfiguration();

    private:
        Utility::ConfigurationGroup* _configurationGroup;

        QString _name,
            _description;

        AbstractPluginManager* _manager;
        PluginModel *_model,
            *_loadedOnlyModel;
};

/**
 * @brief Plugin manager store item
 *
 * Provides access to templated (non-abstract) plugin manager.
 */
template<class Interface> class PluginManagerStore::Item: public PluginManagerStore::AbstractItem {
    public:
        /**
         * @brief Constructor
         *
         * Items are accessible via @ref PluginManagerStore member functions,
         * there is no need to construct them manually.
         * @todo Proper parenting of PluginManager
         */
        inline Item(Utility::ConfigurationGroup* configurationGroup, const QString& name, const QString& description, QObject* parent = 0): AbstractItem(configurationGroup, name, description, new PluginManager<Interface>(configurationGroup->value<std::string>("__dir")), parent) {
            _manager = static_cast<PluginManager<Interface>* >(AbstractItem::manager());
        }

        /** @brief Plugin manager */
        inline PluginManager<Interface>* manager() { return _manager; }

    private:
        PluginManager<Interface>* _manager;
};

}}

#endif
