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
@brief Session manager

<p>Session is used to hold current application state, so when the user exits the
application and then starts it later, the state can be restored. Currently saved
properties are:</p>
<ul>
<li>used map view</li>
<li>active raster model</li>
<li>whether online maps are enabled</li>
<li>opened packages</li>
<li>actual position and zoom</li>
<li>current layer</li>
<li>enabled overlays</li>
</ul>
<p>If automatic session loading is enabled in configuration, last
active session id is retrieved from configuration and last active session can
be then loaded with load(). If there was no last active session, last active
session id is set to default session. On exit current state is saved into active
session or into default session, if no other session is active. The default
session cannot be renamed or deleted.</p>

@configuration

<p>All session configuration is stored in configuration group given to the
constructor.</p>
<pre>
# Boolean whether to load previous session automatically on startup:
loadAutomatically=true

# Current session (group ID, 0 is default, 1 is first session etc.)
default=0

# Default session
[default]

# Used map view plugin
mapView=

# Used raster model plugin
rasterModel=

# Whether online maps are enabled
online=

# Loaded map packages
package=
package=

# Current GPS coordinates and zoom
coordinates=
zoom=

# Map layer and overlays
layer=
overlay=
overlay=
</pre>
*/
class SessionManager: public QObject {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param configuration     Session configuration
         * @param parent            Parent object
         *
         * Initializes manager with session list and if automatic loading on
         * startup is enabled, loads previously active session ID. Previous
         * active session can be then loaded with load().
         */
        SessionManager(Utility::ConfigurationGroup* configuration, QObject* parent = 0);

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
        inline void load() { if(_current != -1) load(_current); }

        /**
         * @brief Load session
         * @param id        Session ID. @c 0 is default session, other sessions
         *      have higher IDs, see names(). If session with given ID doesn't
         *      exist, the function does nothing. If given session is currently
         *      loaded, this call reverts it to saved state.
         *
         * Emits currentChanged().
         */
        void load(unsigned int id);

        /**
         * @brief Save current state to current session
         *
         * Convenience function, same as calling <tt>save(current())</tt>.
         * @see save(unsigned int)
         */
        inline void save() { save(_current); }

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
        Utility::ConfigurationGroup* conf;
        Utility::ConfigurationGroup* defaultSession;
        std::vector<Utility::ConfigurationGroup*> sessions;
        QStringList _names;

        int _current;
        bool loaded;
};

}}

#endif
