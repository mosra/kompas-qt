#ifndef Kompas_QtGui_PluginDialog_h
#define Kompas_QtGui_PluginDialog_h
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
 * @brief Class Kompas::QtGui::PluginDialog
 */

#include "AbstractConfigurationDialog.h"

#include <QtCore/QModelIndex>

#include "AbstractPluginManager.h"
#include "PluginManagerStore.h"

class QDialogButtonBox;
class QTabWidget;
class QLabel;
class QLineEdit;
class QDataWidgetMapper;
class QPushButton;

namespace Kompas { namespace QtGui {

/**
 * @brief Plugin settings dialog
 *
 * Provides viewing and managing plugins.
 * @bug Crash while reloading mapview plugin dir
 */
class PluginDialog: public AbstractConfigurationDialog {
    Q_OBJECT

    public:
        /** @copydoc AbstractConfigurationDialog::AbstractConfigurationDialog */
        PluginDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

    protected:
        class Tab;

    private:
        QTabWidget* tabs;
};

/**
 * @brief %Tab in Plugin dialog
 *
 * Shows table with all plugin and after selecting any row detailed information
 * about that plugin.
 */
class PluginDialog::Tab: public AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param pluginManagerStoreItem    Item of plugin manager store
         * @param parent                    Parent widget
         * @param f                         Window flags
         */
        Tab(PluginManagerStore::AbstractItem* pluginManagerStoreItem, QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        virtual void reset();
        virtual void restoreDefaults();
        virtual void save();

    private slots:
        virtual void setPluginDir();

        void reloadPluginDirectory();

        void loadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);
        void unloadAttempt(const std::string& name, AbstractPluginManager::LoadState before, AbstractPluginManager::LoadState after);

        void setCurrentRow(const QModelIndex& index);
        void reloadCurrentPlugin();

    private:
        PluginManagerStore::AbstractItem* _pluginManagerStoreItem;

        QDataWidgetMapper* mapper;
        QPushButton* reloadPluginButton;

        QLineEdit* pluginDir;

        QLabel *loadStateLabel, *loadState,
            *descriptionLabel, *description,
            *authorsLabel, *authors,
            *dependsLabel, *depends,
            *usedByLabel, *usedBy,
            *replacesLabel, *replaces,
            *replacedWithLabel, *replacedWith;
};

}}

#endif
