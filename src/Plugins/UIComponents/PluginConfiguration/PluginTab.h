#ifndef Kompas_Plugins_UIComponents_PluginTab_h
#define Kompas_Plugins_UIComponents_PluginTab_h
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
 * @brief Class Kompas::Plugins::UIComponents::PluginTab
 */

#include "AbstractConfigurationWidget.h"

#include <QtCore/QModelIndex>

#include "AbstractPluginManager.h"
#include "PluginManagerStore.h"

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QDataWidgetMapper;
class QPushButton;

namespace Kompas { namespace Plugins { namespace UIComponents {

/**
 * @brief Tab in Plugin dialog
 *
 * Shows table with all plugin and after selecting any row detailed information
 * about that plugin.
 */
class PluginTab: public QtGui::AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param pluginManagerStoreItem    Item of plugin manager store
         * @param parent                    Parent widget
         * @param f                         Window flags
         */
        PluginTab(QtGui::PluginManagerStore::AbstractItem* pluginManagerStoreItem, QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        virtual void reset();
        virtual void restoreDefaults();
        virtual void save();

    private slots:
        virtual void setPluginDir();

        void reloadPluginDirectory();

        void loadAttempt(const std::string& name, int before, int after);
        void unloadAttempt(const std::string& name, int before, int after);

        void setCurrentRow(const QModelIndex& index);
        void reloadCurrentPlugin();

    private:
        QtGui::PluginManagerStore::AbstractItem* _pluginManagerStoreItem;

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

}}}

#endif
