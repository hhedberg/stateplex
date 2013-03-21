/*
 * Stateplex - A server-side actor model library.
 *
 * core/actor.cpp
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

#include "actor.h"

namespace Stateplex {

/**
 * Function that checks if the timeout on a actor is largers than the given milliseconds.
 * If the milliseconds are not bigger the function calls for a function that has been set earlier.
 * to handle the timeouts and removes timeouts.
 * 
 * @param milliseconds	milliseconds that are used for comparison.
 * @return mAlive	true, if the actors is still alive after the handling, else false.
 */

bool Actor::handleTimeouts(unsigned long milliseconds)
{
	for (ListIterator<Timeout> iterator(&mTimeouts); iterator.hasCurrent(); iterator.subsequent()) {
		Timeout *timeout = iterator.current();
		if (timeout->milliseconds() > milliseconds)
			break;
		timeout->invokeHandler();
		timeout->remove();
	}

	return mAlive;
}

/**
 * Function that goes through all incoming messages and 
 * calls for a function that earlier to handle the messages and removes messages.
 * 
 * @param milliseconds
 * @return mAlive	true, if the actors is still alive after the handling, else false.
 */

bool Actor::handleMessages(unsigned long milliseconds)
{
	handleTimeouts(milliseconds);

	for (ListIterator<Message> iterator(&mIncomingMessages); iterator.hasCurrent(); iterator.subsequent()) {
		Message *message = iterator.current();
		message->invokeHandler();
		message->remove();
	}

	return mAlive;
}

/**
 * Function that adds timeout to a list.
 * 
 * @param *timeout	pointer to a timeout to be added.
 */

void Actor::addTimeout(Timeout *timeout)
{
	for (ListIterator<Timeout> iterator(&mTimeouts); iterator.hasCurrent(); iterator.subsequent()) {
		Timeout *existing = iterator.current();
		if (timeout->milliseconds() < existing->milliseconds()) {
			timeout->addBefore(existing);
			return;
		}
	}
	mTimeouts.addTail(timeout);
}

}
