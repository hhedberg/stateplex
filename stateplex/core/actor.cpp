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
 * Function that checks if the timeout on a actor is larger than the given milliseconds.
 * If the milliseconds is not larger, the function calls for object handlers function and
 * then removes the timeout.
 * 
 * @param milliseconds	milliseconds that are used for comparison.
 * @return	true, if the actors is still alive after the handling, else false.
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
 * calls for a object handlers function that to handle the messages and then removes the messages.
 * 
 * @param milliseconds	milliseconds used for handling timeouts.
 * @return	true, if the actors is still alive after the handling, else false.
 */

bool Actor::handleMessages(unsigned long milliseconds)
{
	handleTimeouts(milliseconds);

	for (ListIterator<Message<Actor> > iterator(&mIncomingMessages); iterator.hasCurrent(); iterator.subsequent()) {
		Message<Actor> *message = iterator.current();
		message->handle(message->actor(), message->mReceiver);
		message->remove();
	}

	return mAlive;
}

/**
 * Function that adds specific timeout to the end of actors timeout list.
 * 
 * @param *timeout	pointer to a timeout to be added.
 * @return		returns void if timeout is smaller that the existing timeout, else returns void at the
 * 			end of the function.
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
