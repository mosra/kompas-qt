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

#include "PluginModel.h"

#include <QtCore/QStringList>

#include "PluginManager.h"

using namespace std;

namespace Kompas { namespace QtGui {

PluginModel::PluginModel(AbstractPluginManager* _manager, int flags, QObject* parent): QAbstractTableModel(parent), manager(_manager), _flags(flags) {
    connect(manager,
            SIGNAL(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
            SLOT(loadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));
    connect(manager,
            SIGNAL(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)),
            SLOT(unloadAttempt(std::string,AbstractPluginManager::LoadState,AbstractPluginManager::LoadState)));
    connect(manager,
            SIGNAL(pluginMetadataReloaded(std::string)),
            SLOT(reloadPluginMetadata(std::string)));
    connect(manager,
            SIGNAL(pluginDisappeared(std::string)),
            SLOT(removePlugin(std::string)));
    connect(manager,
            SIGNAL(pluginDirectoryReloaded()),
            SLOT(reload()));

    reload();
}

void PluginModel::reload() {
    beginResetModel();
    plugins.clear();
    vector<string> _plugins = manager->pluginList();

    for(vector<string>::const_iterator it = _plugins.begin(); it != _plugins.end(); ++it) {
        if((_flags & LoadedOnly) && !(manager->loadState(*it) & (AbstractPluginManager::IsStatic|AbstractPluginManager::LoadOk))) continue;

        plugins.append(PluginMetadata(*it, manager->loadState(*it), manager->metadata(*it)));
    }
    endResetModel();
}

int PluginModel::findPlugin(const QString& name) const {
    int found = -1;
    for(int i = 0; i != plugins.size(); ++i) if(plugins[i].plugin == name) {
        found = i;
        break;
    }
    return found;
}

QVariant PluginModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case LoadState:     return tr("Load state");
            case Plugin:        return tr("Plugin");
            case Name:          return tr("Name");
            case Description:   return tr("Description");
            case Authors:       return tr("Authors");
            case Version:       return tr("Version");
            case Depends:       return tr("Depends on");
            case UsedBy:        return tr("Used by");
            case Replaces:      return tr("Replaces");
            case ReplacedWith:  return tr("Can be replaced with");
        }
    }

    return QVariant();
}

QVariant PluginModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    const PluginMetadata& metadata = plugins[index.row()];

    /* Load state */
    if(index.column() == LoadState) {
        if(role == Qt::CheckStateRole) {
            if(metadata.loadState & (AbstractPluginManager::LoadOk|AbstractPluginManager::UnloadFailed|AbstractPluginManager::IsRequired|AbstractPluginManager::IsStatic))
                return Qt::Checked;
            else
                return Qt::Unchecked;

        } else if(role == Qt::DisplayRole || role == Qt::EditRole) switch(metadata.loadState) {
            case AbstractPluginManager::NotFound:
                return tr("Not found");
            case AbstractPluginManager::WrongPluginVersion:
                return tr("Wrong plugin version");
            case AbstractPluginManager::WrongInterfaceVersion:
                return tr("Wrong interface version");
            case AbstractPluginManager::WrongMetadataFile:
                return tr("Error in metadata file");
            case AbstractPluginManager::UnresolvedDependency:
                return tr("Unresolved dependency");
            case AbstractPluginManager::LoadFailed:
                return tr("Loading failed");
            case AbstractPluginManager::LoadOk:
                return tr("Loaded");
            case AbstractPluginManager::NotLoaded:
                return tr("Not loaded");
            case AbstractPluginManager::UnloadFailed:
                return tr("Unload failed");
            case AbstractPluginManager::IsRequired:
                return tr("Is required by another");
            case AbstractPluginManager::IsStatic:
                return tr("Static plugin");
            case AbstractPluginManager::IsUsed:
                return tr("Is already used");

        } else if(role == Qt::UserRole) return metadata.loadState;

    } else if(role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    else if(index.column() == Plugin)           return metadata.plugin;
    else if(index.column() == Name)             return QString::fromStdString(*metadata.name);
    else if(index.column() == Description)      return QString::fromStdString(*metadata.description);
    else if(index.column() == Authors)          return metadata.authors;
    else if(index.column() == Version)          return metadata.version;
    else if(index.column() == Depends)          return metadata.depends;
    else if(index.column() == UsedBy)           return metadata.usedBy;
    else if(index.column() == Replaces)         return metadata.replaces;
    else if(index.column() == ReplacedWith)     return metadata.replacedWith;

    return QVariant();
}

Qt::ItemFlags PluginModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::ItemIsEnabled;

    /* Only load state column is checkable */
    if(index.column() == LoadState) {
        /* Static plugins are disabled */
        if(plugins[index.row()].loadState == AbstractPluginManager::IsStatic)
            return (QAbstractTableModel::flags(index)|Qt::ItemIsUserCheckable)&(~Qt::ItemIsEnabled);
        else
            return QAbstractTableModel::flags(index)|Qt::ItemIsUserCheckable;
    }

    return QAbstractItemModel::flags(index);
}

bool PluginModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if((_flags & LoadedOnly) || !index.isValid() || index.column() != LoadState || role != Qt::CheckStateRole)
        return false;

    AbstractPluginManager::LoadState loadState = plugins[index.row()].loadState;
    string plugin = plugins[index.row()].plugin.toStdString();

    /* Static plugins cannot be edited */
    if(loadState == AbstractPluginManager::IsStatic)
        return false;

    /* Unload plugin */
    if(loadState & (AbstractPluginManager::LoadOk|AbstractPluginManager::UnloadFailed|AbstractPluginManager::IsRequired)) {
        if(manager->unload(plugin) != AbstractPluginManager::NotLoaded)
            return false;

    /* Load plugin */
    } else if(manager->load(plugin) != AbstractPluginManager::LoadOk)
        return false;

    return true;
}

void PluginModel::loadAttempt(const std::string& plugin, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    /* Plugin was loaded */
    if(!(before & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)) &&
       (after & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))) {

        /* Add to list, if displaying only loaded plugins */
        if(_flags & LoadedOnly) {
            beginInsertRows(QModelIndex(), plugins.size(), plugins.size());
            /** @todo Insert to right place (alphabetically sorted) */
            plugins.append(PluginMetadata(plugin, after, manager->metadata(plugin)));
            endInsertRows();

        /* Or just emit signal about data change */
        } else {
            /* Find the name in list */
            int found = findPlugin(QString::fromStdString(plugin));
            if(found == -1) return;

            /* Update plugin state */
            plugins[found].loadState = manager->loadState(plugin);
            emit dataChanged(index(found, LoadState), index(found, LoadState));
        }
    }
}

void PluginModel::unloadAttempt(const std::string& plugin, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after) {
    /** @bug When trying to unload used plugin the plugin is removed from LoadedOnly model! ... check all states */
    /* Plugin was unloaded */
    if((before & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic)) &&
       !(after & (AbstractPluginManager::LoadOk|AbstractPluginManager::IsStatic))) {

        /* Find the name in list */
        int found = findPlugin(QString::fromStdString(plugin));
        if(found == -1) return;

        /* Remove from list, if displaying only loaded plugins */
        if(_flags & LoadedOnly) {
            beginRemoveRows(QModelIndex(), found, found);
            plugins.removeAt(found);
            endRemoveRows();

        /* Or just emit signal about data change */
        } else {
            /* Update plugin state */
            plugins[found].loadState = manager->loadState(plugin);
            emit dataChanged(index(found, LoadState), index(found, LoadState));
        }
    }
}

void PluginModel::reloadPluginMetadata(const std::string& plugin) {
    /* Find the name in list */
    int found = findPlugin(QString::fromStdString(plugin));
    if(found == -1) return;

    /* Update plugin metadata */
    plugins.replace(found, PluginMetadata(plugin, manager->loadState(plugin), manager->metadata(plugin)));
    emit dataChanged(index(found, 0), index(found, columnCount()-1));
}

void PluginModel::removePlugin(const std::string& plugin) {
    /* Find the name in list */
    int found = findPlugin(QString::fromStdString(plugin));
    if(found == -1) return;

    /* Remove from list, if displaying only loaded plugins */
    if(_flags & LoadedOnly) {
        beginRemoveRows(QModelIndex(), found, found);
        plugins.removeAt(found);
        endRemoveRows();
    }
}

PluginModel::PluginMetadata::PluginMetadata(const std::string& _plugin, AbstractPluginManager::LoadState _loadState, const Kompas::PluginManager::PluginMetadata* metadata) {
    plugin = QString::fromStdString(_plugin);
    loadState = _loadState;
    name = metadata->name();
    description = metadata->description();
    version = QString::fromStdString(metadata->version());

    QStringList list;
    vector<string> temp = metadata->authors();
    for(vector<string>::const_iterator it = temp.begin(); it != temp.end(); ++it)
        list << QString::fromStdString(*it);
    authors = list.join(", ");

    list.clear();
    temp = metadata->depends();
    for(vector<string>::const_iterator it = temp.begin(); it != temp.end(); ++it)
        list << QString::fromStdString(*it);
    depends = list.join(", ");

    list.clear();
    temp = metadata->usedBy();
    for(vector<string>::const_iterator it = temp.begin(); it != temp.end(); ++it)
        list << QString::fromStdString(*it);
    usedBy = list.join(", ");

    list.clear();
    temp = metadata->replaces();
    for(vector<string>::const_iterator it = temp.begin(); it != temp.end(); ++it)
        list << QString::fromStdString(*it);
    replaces = list.join(", ");

    list.clear();
    temp = metadata->replacedWith();
    for(vector<string>::const_iterator it = temp.begin(); it != temp.end(); ++it)
        list << QString::fromStdString(*it);
    replacedWith = list.join(", ");
}

}}
