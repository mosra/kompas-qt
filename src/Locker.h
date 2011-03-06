#ifndef Kompas_QtGui_Locker_h
#define Kompas_QtGui_Locker_h
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
 * @brief Class Kompas::QtGui::Locker
 */

#include <QtCore/QReadWriteLock>

namespace Kompas { namespace QtGui {

/**
@brief Convenience class for less error-prone usage of data shared among threads

Basically an wrapper around QReadWriteLock, ensures that data can be accessed
only if the lock is locked. On construction takes pointer to the data and pointer
to QReadWriteLock instance. When the data are first accessed via
@ref operator(), it locks the QReadWriteLock and returns pointer to the data.
Upon destruction the lock is automatically unlocked, the lock can be also
unlocked manually with @ref unlock(). After unlocking the data cannot be
accessed (@ref operator() returns always 0).

If the locker is created with @c const template parameter, the data are locked
for reading (@c QReadWriteLock::lockForRead()), otherwise they are locked for
writing (@c QReadWriteLock::lockForWrite()).

Example usage:
@code
// Functions for creating the lock
Locker<const MyData> lockForRead() {
    return Locker<const MyData>(myData, readWriteLock);
}
Locker<MyData> lockForWrite() {
    return Locker<MyData>(myData, readWriteLock);
}

// Simple data access (notice the two pairs of brackets). The locker instance
// is destroyed (and lock unlocked) right after function call.
lockForWrite()()->setFoo("bar");

// Data access. Lock is unlocked on 'data' destruction or after calling
// @ref unlock().
Locker<const MyData> data = lockForRead();
data()->doSomething();
data()->doSomethingAnother();
@endcode
*/
template<class T> class Locker {
    public:
        /** @brief Locker state */
        enum State {
            Fresh,      /**< Right after construction, data hasn't been accessed yet. */
            Locked,     /**< Data has been acessed, the lock is locked. */
            Unlocked    /**< Locker is unlocked with @ref unlock(), data cannot be accessed anymore. */
        };

        /**
         * @brief Constructor
         * @param data          Pointer to data
         * @param lock          Pointer to read-write lock instance
         */
        inline Locker(T* data, QReadWriteLock* lock): _state(Fresh), _data(data), _lock(lock) {}

        /**
         * @brief Copy constructor
         *
         * If the original locker is not in fresh state, this instance will
         * be set to @ref Unlocked (and thus unusable) state.
         * @attention Avoid using original and copied locker simultaenously.
         *      When copying the locker, always destroy the original instance.
         */
        inline Locker(const Locker<T>& other): _state(other.state() == Fresh ? Fresh : Unlocked), _data(other._data), _lock(other._lock) {}

        /**
         * @brief Destructor
         *
         * Calls @ref unlock().
         */
        inline ~Locker() { unlock(); }

        /** @brief Locker state */
        inline State state() const { return _state; }

        /**
         * @brief Assignment operator
         *
         * If the original locker is not in fresh state, this instance will
         * be set to @ref Unlocked (and thus unusable) state.
         * @attention Avoid using original and copied locker simultaenously.
         *      When copying the locker, always destroy the original instance.
         */
        Locker<T>& operator=(const Locker<T>& other) {
            /* If other locker is not in fresh state, don't allow having both locks working at once */
            _state = other.state() == Fresh ? Fresh : Unlocked;
            _data = other._data;
            _lock = other._lock;
        }

        /**
         * @brief Data access
         *
         * If the locker is in @c Locked state, returns pointer to the data,
         * otherwise returns 0.
         */
        T* operator()() {
            /* If lock is in fresh state, lock it */
            if(_state == Fresh) {
                _lock->lockForWrite();
                _state = Locked;
            }

            return _state == Locked ? _data : 0;
        }

        /**
         * @brief Unlock the locker
         *
         * After unlocking the data cannot be accessed anymore.
         */
        inline void unlock() {
            if(_state != Locked) return;
            _state = Unlocked;
            _lock->unlock();
        }

    private:
        State _state;
        T* _data;
        QReadWriteLock* _lock;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class T> class Locker<const T> {
    public:
        enum State {
            Fresh,
            Locked,
            Unlocked
        };

        inline Locker(const T* data, QReadWriteLock* lock): _state(Fresh), _data(data), _lock(lock) {}

        inline Locker(const Locker<const T>& other): _state(other.state() == Fresh ? Fresh : Unlocked), _data(other._data), _lock(other._lock) {}

        inline ~Locker() { unlock(); }

        inline State state() const { return _state; }

        Locker<const T>& operator=(const Locker<const T>& other) {
            /* If other locker is not in fresh state, don't allow having both locks working at once */
            _state = other.state() == Fresh ? Fresh : Unlocked;
            _data = other._data;
            _lock = other._lock;
        }

        const T* operator()() {
            /* If lock is in fresh state, lock it */
            if(_state == Fresh) {
                _lock->lockForRead();
                _state = Locked;
            }

            return _state == Locked ? _data : 0;
        }

        inline void unlock() {
            if(_state != Locked) return;
            _state = Unlocked;
            _lock->unlock();
        }

    private:
        State _state;
        const T* _data;
        QReadWriteLock* _lock;
};
#endif

}}

#endif
