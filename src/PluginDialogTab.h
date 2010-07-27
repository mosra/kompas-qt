#ifndef Map2X_QtGui_PluginDialogTab_h
#define Map2X_QtGui_PluginDialogTab_h
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
 * @brief Class Map2X::QtGui::PluginDialogtab
 */

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>

class QLabel;
class QLineEdit;
class QDataWidgetMapper;

namespace Map2X {

namespace PluginManager {
    class AbstractPluginManager;
}

namespace QtGui {

class MainWindow;
class PluginModel;

/**
 * @brief Tab in Plugin dialog
 *
 * Shows table with all plugin and after selecting any row detailed information
 * about that plugin.
 * @todo Dir dialog for selecting plugin dir
 */
class PluginDialogTab: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _mainWindow           Pointer to main window instance
         * @param _configurationKey     Key name for storing plugin configuration
         * @param _manager              Pointer to PluginManager
         * @param _categoryDescription  Description of current plugin category
         * @param parent                Parent widget
         * @param f                     Window flags
         */
        PluginDialogTab(MainWindow* _mainWindow, const std::string& _configurationKey, PluginManager::AbstractPluginManager* _manager, const QString& _categoryDescription, QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        void save();                    /**< @brief Save changes to configuration */
        void reset();                   /**< @brief Reset changes */
        void restoreDefaults();         /**< @brief Load default configuration */

    private:
        MainWindow* mainWindow;
        std::string configurationKey;

        PluginManager::AbstractPluginManager* manager;
        PluginModel* model;
        QDataWidgetMapper* mapper;

        QLineEdit* pluginDir;

        QLabel *loadState,
            *description,
            *depends,
            *replaces,
            *conflicts;
};

}}

#endif
