#ifndef Map2X_QtGui_AbstractPluginMenuView_h
#define Map2X_QtGui_AbstractPluginMenuView_h
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
 * @brief Class Map2X::QtGui::AbstractPluginMenuView
 */

#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "AbstractPluginManager.h"

namespace Map2X { namespace QtGui {

/**
 * @brief Creating dynamic menus depending on actually loaded plugins
 *
 * This class allows easily creating e.g. an Export menu, where every item will
 * do basically the same, but always use a different plugin for the action. By
 * default all plugins are inserted into the menu, but subclasses can do some
 * filtering in their createMenuAction() implementation and add only some
 * plugins to the menu.
 */
class AbstractPluginMenuView: public QObject {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _manager      Plugin manager
         * @param _menu         Menu to which add items
         * @param _before       Menu item before which add items. If set to 0,
         *      the items will be put at the end of menu.
         * @param parent        Parent widget
         */
        inline AbstractPluginMenuView(AbstractPluginManager* _manager, QMenu* _menu, QAction* _before = 0, QObject* parent = 0): manager(_manager), menu(_menu), before(_before) {}

        /**
         * @brief Destructor
         */
        inline virtual ~AbstractPluginMenuView() {}

    public slots:
        /**
         * @brief Update menu with new items
         *
         * Goes through all loaded plugins and creates menu items for them via
         * createMenuAction().
         * @attention It has to be called explicitly after instantiation,
         * otherwise the menu will be empty.
         * @todo Connect plugin manager updates with this.
         */
        virtual void update();

    protected slots:
        /**
         * @brief Trigger given action
         * @param action        Action
         *
         * Called after clicking any menu item in given menu.
         * @attention As the menu can have actions that were not created with
         * this class, the implementation should count on that.
         */
        virtual void trigger(QAction* action) = 0;

    protected:
        AbstractPluginManager* manager;       /**< @brief Plugin manager */

        /**
         * @brief Create menu action
         * @param pluginName    Name of the plugin for which create actions.
         * @return Pointer to newly created action. If returned 0, nothing will
         *      be added to menu.
         *
         * Implementation in subclasses creates a menu item with some caption
         * and/or icon, possibly filter out some plugins, which would not be
         * displayed in the menu.
         */
        virtual QAction* createMenuAction(const std::string& pluginName) = 0;

    private:
        QMenu* menu;
        QAction* before;
};

}}

#endif
