#ifndef Kompas_QtGui_PluginModel_h
#define Kompas_QtGui_PluginModel_h
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
 * @brief Class Kompas::QtGui::PluginModel
 */

#include <QtCore/QAbstractTableModel>

#include "AbstractPluginManager.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Model for viewing and managing plugins
 * @bug Properly handle plugin disappearing
 */
class PluginModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Flags */
        enum Flags {
            LoadedOnly  = 0x01      /**< @brief Display only loaded plugins */
        };

        /** @brief Columns */
        enum Column {
            LoadState,
            Plugin,
            Name,
            Description,
            Authors,
            Version,
            Depends,
            UsedBy,
            Replaces,
            ReplacedWith
        };

        /**
         * @brief Constructor
         *
         * @param _manager      Pointer to PluginManager
         * @param flags         Flags
         * @param parent        Parent object
         */
        PluginModel(AbstractPluginManager* _manager, int flags = 0, QObject* parent = 0);

        /**
         * @brief Find plugin with specific name
         * @param plugin        Plugin
         * @return Row index of given plugin or -1 if the plugin is not found
         *      in this model.
         */
        int findPlugin(const QString& plugin) const;

        /** @brief Row count */
        int rowCount(const QModelIndex& parent = QModelIndex()) const { return plugins.size(); }

        /** @brief Column count */
        int columnCount(const QModelIndex& parent = QModelIndex()) const { return 10; }

        /** @brief Header data access */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /** @brief Data read access */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /** @brief Item flags */
        Qt::ItemFlags flags(const QModelIndex& index) const;

        /** @brief Data write access */
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    public slots:
        /** @brief Reload data from PluginManager */
        void reload();

    private slots:
        void loadAttempt(const std::string& plugin, int before, int after);
        void unloadAttempt(const std::string& plugin, int before, int after);
        void reloadPluginMetadata(const std::string& plugin);
        void reloadPluginUsedByMetadata(const std::string& plugin);
        void removePlugin(const std::string& plugin);

    private:
        struct PluginMetadata {
            PluginMetadata(const std::string& _plugin, AbstractPluginManager::LoadState _loadState, const Corrade::PluginManager::PluginMetadata* metadata);

            const std::string *name,
                *description;
            QString plugin,
                authors,
                version,
                depends,
                usedBy,
                replaces,
                replacedWith;

            AbstractPluginManager::LoadState loadState;
        };

        AbstractPluginManager* manager;
        int _flags;
        QList<PluginMetadata> plugins;
};

}}

#endif
