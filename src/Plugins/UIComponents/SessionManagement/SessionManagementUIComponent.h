#ifndef Kompas_Plugins_UIComponents_SessionManagementUIComponent_h
#define Kompas_Plugins_UIComponents_SessionManagementUIComponent_h
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
 * @brief Class Kompas::Plugins::UIComponents::SessionManagementUIComponent
 */

#include "AbstractUIComponent.h"

class QAction;
class QMenu;

namespace Kompas { namespace Plugins { namespace UIComponents {

/** @brief Session management */
class SessionManagementUIComponent: public QtGui::AbstractUIComponent {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractUIComponent::AbstractUIComponent */
        SessionManagementUIComponent(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        inline const QList<QAction*>* actions(ActionCategory category) const {
            if(category == QtGui::AbstractUIComponent::Sessions)
                return &_actions;
            return 0;
        }

    private slots:

        /**
         * @brief Reflect current session change in the UI
         *
         * Changes window title and disables rename/delete items in
         * session menu if default session is loaded.
         */
        void currentSessionChange();

        /**
         * @brief Create new session
         *
         * Shows dialog asking for session name, creates new session and
         * switches to it.
         */
        void newSession();

        /**
         * @brief Rename current session
         *
         * Shows dialog asking for session name. If current session is default
         * session, does nothing.
         */
        void renameSession();

        /**
         * @brief Delete session
         *
         * Ask whether to delete, deletes current session and switches to
         * default session. If current session is default session, does nothing.
         */
        void deleteSession();

    private:
        QList<QAction*> _actions;

        QAction *deleteSessionAction,
            *renameSessionAction;

        QMenu *sessionMenu;
};

}}}

#endif
