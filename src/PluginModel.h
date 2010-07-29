#ifndef Map2X_QtGui_PluginModel_h
#define Map2X_QtGui_PluginModel_H
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
 * @brief Class Map2X::QtGui::PluginModel
 */

#include <vector>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>

namespace Map2X {

namespace PluginManager {
    class AbstractPluginManager;
}

namespace QtGui {

/**
 * @brief Model for viewing and managing plugins
 * @todo Required by, conflict with etc.
 * @todo Don't regenerate lists on every data request
 */
class PluginModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Flags */
        enum Flags {
            LoadedOnly          /**< @brief Display only loaded plugins */
        };

        /** @brief Columns */
        enum Column {
            CheckState,
            LoadState,
            Plugin,
            Name,
            Description,
            Depends,
            Replaces,
            Conflicts
        };

        /**
         * @brief Constructor
         *
         * @param _manager      Pointer to PluginManager
         * @param _flags        Flags
         * @param parent        Parent object
         */
        inline PluginModel(PluginManager::AbstractPluginManager* _manager, int _flags = 0, QObject* parent = 0):
            QAbstractTableModel(parent), manager(_manager), flags(_flags) { reload(); }

        /** @brief Reload data from PluginManager */
        void reload();

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const { return nameList.size(); }
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const { return 8; }
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    private:
        PluginManager::AbstractPluginManager* manager;
        int flags;
        std::vector<std::string> nameList;
};

}}

#endif
