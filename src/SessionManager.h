#ifndef Kompas_QtGui_SessionManager_h
#define Kompas_QtGui_SessionManager_h
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
 * @brief Class Kompas::QtGui::SessionManager
 */

#include <QtCore/QStringList>

#include "Utility/Configuration.h"

namespace Kompas { namespace QtGui {

/**
 * @brief Session manager
 */
class SessionManager: public QObject {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param filename      Filename of session configuration file
         *
         * Initializes manager with session list and sets current ID to last
         * active session ID.
         */
        SessionManager(const QString& filename);

        /**
         * @brief Destructor
         *
         * Saves current session ID to the configuration file. Note that current
         * session is not saved, it has to be done explicitly.
         */
        ~SessionManager();

        /**
         * @brief List of session names
         *
         * Name under @c n-th index is session with ID @c n+1. Default session
         * has ID @c 0.
         */
        inline QStringList names() const { return _names; }

        /**
         * @brief ID of currently active session
         *
         * On startup the ID is set to last active session or 0 (default
         * session), if no session was previously loaded. The session is
         * not loaded. Current ID is changed after calling load().
         */
        inline unsigned int current() const { return _current; }

        /**
         * @brief Whether currently active session is loaded
         *
         * Set to true after first calling load().
         */
        inline bool isLoaded() const { return loaded; }

        /**
         * @brief Load last used session
         *
         * Convenience function, same as calling <tt>load(current())</tt>.
         * @see load(unsigned int)
         */
        inline void load() { load(current()); }

        /**
         * @brief Load session
         * @param id        Session ID. @c 0 is default session, other sessions
         *      have higher IDs, see names(). If session with given ID doesn't
         *      exist, the function does nothing. If given session is currently
         *      loaded, this call reverts it to saved state.
         *
         * Loads given session, that is:
         * - used map view
         * - active raster moder
         * - whether online maps are enabled
         * - opened packages
         * - actual position
         * - current layer
         * - enabled overlays
         *
         * Emits currentChanged().
         */
        void load(unsigned int id);

        /**
         * @brief Save current state to current session
         *
         * Convenience function, same as calling <tt>saveAs(current())</tt>.
         * @see save(unsigned int)
         */
        inline void save() { save(current()); }

        /**
         * @brief Save current state
         * @param id        Id of session where save the session. If session
         *      with given ID doesn't exist, the function does nothing.
         *
         * Saves current state to session with given ID and sets it as current.
         * See load() for list of what is saved to the session.
         *
         * Emits currentChanged().
         */
        void save(unsigned int id);

        /**
         * @brief Create new session
         * @param name      Session name
         * @return ID of newly created session
         *
         * Emits namesChanged().
         */
        unsigned int newSession(const QString& name);

        /**
         * @brief Rename session
         * @param id        Session ID. If session with given ID doesn't exist,
         *      the function does nothing. Default session (with ID @c 0) cannot
         *      be renamed.
         * @param name      Session name
         *
         * Emits namesChanged().
         */
        void renameSession(unsigned int id, const QString& name);

        /**
         * @brief Delete session
         * @param id        Session ID. If session with given ID doesn't exist,
         *      the function does nothing. Default session (with ID @c 0) cannot
         *      be deleted.
         *
         * Emits namesChanged().
         */
        void deleteSession(unsigned int id);

    signals:
        /**
         * @brief Session list changed
         *
         * Emitted when a session is created, renamed or deleted.
         */
        void namesChanged();

        /**
         * @brief Current session changed
         *
         * Emitted when current session ID is changed.
         */
        void currentChanged(unsigned int id);

    private:
        Utility::Configuration conf;

        Utility::ConfigurationGroup* defaultSession;
        std::vector<Utility::ConfigurationGroup*> sessions;
        QStringList _names;

        unsigned int _current;
        bool loaded;
};

}}

#endif
