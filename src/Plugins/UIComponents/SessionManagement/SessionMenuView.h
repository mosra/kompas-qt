#ifndef Kompas_Plugins_UIComponents_SessionMenuView_h
#define Kompas_Plugins_UIComponents_SessionMenuView_h
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
 * @brief Class Kompas::Plugins::UIComponents::SessionMenuView
 */

#include <QtCore/QObject>
#include <QtCore/QHash>

class QActionGroup;
class QMenu;
class QAction;

namespace Kompas {

namespace QtGui {
    class SessionManager;
}

namespace Plugins { namespace UIComponents {

/**
 * @brief Menu view for sessions
 *
 * Displays default session and all other sessions in given menu.
 */
class SessionMenuView: public QObject {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _manager      Session manager
         * @param _menu         Menu which to fill with session list
         * @param parent        Parent object
         *
         * Calls updateNames().
         */
        SessionMenuView(QtGui::SessionManager* _manager, QMenu* _menu, QAction* before, QObject* parent);

    public slots:
        /**
         * @brief Update session names
         *
         * Connected to SessionManager::namesChanged(). If any session is
         * loaded, it is marked with checkbox.
         */
        void updateNames();

    private slots:
        /**
         * @brief Update current session
         *
         * Connected to SessionManager::currentChanged().
         */
        void updateCurrent(unsigned int id);

        /**
         * @brief Load given session
         */
        void trigger(QAction* action);

    private:
        QtGui::SessionManager* manager;
        QMenu* menu;
        QAction* before;
        QHash<QAction*, unsigned int> sessions;
        QActionGroup* group;
};

}}}

#endif
