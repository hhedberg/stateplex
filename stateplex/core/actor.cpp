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

bool Actor::handleTimeouts(unsigned long milliseconds)
{
	for (ListItem<Timeout> *item = mTimeouts.first(); item; item = mTimeouts.next(item)) {
		if (item->container()->milliseconds() > milliseconds)
			break;
		item->container()->invokeHandler();
	}

	return mAlive;
}

bool Actor::handleMessages(unsigned long milliseconds)
{
	handleTimeouts(milliseconds);

	for (ListItem<Message> *item = mIncomingMessages.first(); item; item = mIncomingMessages.next(item))
		item->container()->invokeHandler();

	return mAlive;
}

void Actor::addTimeout(Timeout *timeout)
{
	for (ListItem<Timeout> *item = mTimeouts.first(); item; item = mTimeouts.next(item)) {
		if (timeout->milliseconds() < item->container()->milliseconds()) {
			timeout->listItem.addBefore(item);
			return;
		}
	}
	mTimeouts.addTail(&timeout->listItem);
}

}
