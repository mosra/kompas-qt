#ifndef Kompas_QtGui_AbstractUIComponent_h
#define Kompas_QtGui_AbstractUIComponent_h
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
 * @brief Class Kompas::QtGui::AbstractUIComponent
 */

#include <QtCore/QObject>
#include <QtCore/QMultiHash>

#include "TranslatablePlugin.h"

class QDockWidget;
class QToolBar;
class QMenuBar;
class QAction;
class QStatusBar;

namespace Kompas { namespace QtGui {

/**
@brief Abstract class for UI component plugins

These plugins provide complete UI presented to user. Every plugin implements
one or more functions to provide either central widget, dock widget, toolbar,
menubar, statusbar or some actions.

Actions are divided to a few categories (see @ref ActionCategory). When the UI
plugin is loaded and instanced, MainWindow goes through all already present
actions and send every one of them with its corresponding category to slot
@ref actionAdded() of newly instanced plugin. Also when a new UI plugin, which
contains actions, is loaded, all these actions are sent to @ref actionAdded()
slots of already loaded plugins.

To be able to dynamically remove UI components without the need for restart,
the canBeDeleted() function is set to true, thus plugin manager deletes all
instances when the plugin is being unloaded.
*/
class AbstractUIComponent: public QObject, public Core::TranslatablePlugin {
    Q_OBJECT

    PLUGIN_INTERFACE("cz.mosra.Kompas.QtGui.AbstractUIComponent/0.2")

    public:
        /** @brief Action category */
        enum ActionCategory {
            Sessions,           /**< Opening, switching, saving and managing sessions */
            Maps,               /**< Opening, closing maps */
            Tools,              /**< Various tools */
            Settings,           /**< Various settings */
            Help                /**< Help */
        };

        /** @copydoc PluginManager::Plugin::Plugin */
        inline AbstractUIComponent(Corrade::PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = ""): TranslatablePlugin(manager, plugin) {}

        inline bool canBeDeleted() { return true; }

        /**
         * @brief Central widget
         *
         * Widget used as central widget in main window (can be only one at
         * a time).
         */
        inline virtual QWidget* centralWidget() const { return 0; }

        /**
         * @brief Dock widget
         * @param area      Area where to place the widget. Always set to
         *      nonzero value when called.
         *
         * Widget docked beside central widget.
         */
        inline virtual QDockWidget* dockWidget(Qt::DockWidgetArea* area) const { return 0; }

        /**
         * @brief Tool bar
         * @param area      Area where to place the tool bar. Aways set to
         *      nonzero value when called.
         */
        inline virtual QToolBar* toolBar(Qt::ToolBarArea* area) const { return 0; }

        /**
         * @brief Menu bar
         *
         * Can be only one at a time.
         */
        inline virtual QMenuBar* menuBar() const { return 0; }

        /**
         * @brief Status bar
         *
         * Can be only one at a time.
         */
        inline virtual QStatusBar* statusBar() const { return 0; }

        /**
         * @brief Actions
         *
         * Returns a list of actions defined with this plugin for given category.
         */
        inline virtual const QList<QAction*>* actions(ActionCategory category) const { return 0; }

    public slots:
        /**
         * @brief An action with specified category can be added
         *
         * Reimplement, if UI component has for example an menu, which can
         * display actions from other UI component plugins.
         */
        virtual void actionAdded(int category, QAction* action) {}
};

}}

#endif
