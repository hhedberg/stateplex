/*
 * Stateplex - A server-side actor model library.
 *
 * core/spinlock.h
 *
 * (c) 2013 Henrik Hedberg <henrik.hedberg@innologies.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Authors: Henrik Hedberg
 */

#ifndef INCLUDED_STATEPLEX_SPINLOCK_H
#define INCLUDED_STATEPLEX_SPINLOCK_H

namespace Stateplex {

class Spinlock {
	volatile int mState;
public:
	Spinlock(bool locked = false);
	bool tryLock();
	void lock();
	void unlock();
};

/*** Inline implementations ***/

/** 
 * Constructor for Spinlock class, sets member variable mState.
 *
 * @param locked	boolean value that determines the value of mState
 */

inline Spinlock::Spinlock(bool locked)
	: mState(locked ? 1 : 0)
{ }

/** 
 * Function that tries to get a lock if mState is 0
 *
 * @return	true if locking succeeded, otherwise false
 */

inline bool Spinlock::tryLock()
{
	return (mState ? false
	              : (__sync_lock_test_and_set(&mState, 1) ? false
		                                             : true));
}

/** 
 * Function that tries to get a lock. Waits until mState is 0 and then
 * tries locking.
 *
 */

inline void Spinlock::lock()
{
	do {
		while (mState);
	} while (__sync_lock_test_and_set(&mState, 1));
}

/** 
 * Function that releases the lock.
 *
 */

inline void Spinlock::unlock()
{
	__sync_lock_release(&mState);
}

}

#endif
