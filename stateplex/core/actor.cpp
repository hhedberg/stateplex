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
 * Function that handles timeouts
 * 
 *
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
 * Function that handles messages
 * 
 *
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
 * Function that adds timeout
 * 
 *
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
