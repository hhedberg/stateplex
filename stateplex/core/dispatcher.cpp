/*
 * Stateplex - A server-side actor model library.
 *
 * core/dispatcher.cpp
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

#include <sys/time.h>
#include <sys/epoll.h>

#include "dispatcher.h"
#include "actor.h"

/* TODO: Parametrise this */
#define MAX_EVENTS 10

namespace Stateplex {

Spinlock Dispatcher::sDispatchLock;

void Dispatcher::run()
{
	struct epoll_event events[MAX_EVENTS];
	int n_events;
	struct timeval tv;
	List<Actor> actors;
	int timeout;
	
	while (mRunning) {
		if (!mOutgoingMessages.isEmpty()) {
			sDispatchLock.lock();
			for (ListItem<Message> *item = mOutgoingMessages.first(); item; item = mOutgoingMessages.next(item)) {
				Actor *receiver = item->container()->receiver;
				receiver->mQueuedMessages.addTail(item);
				activateActor(receiver);
			}
			sDispatchLock.unlock();
		}

		gettimeofday(&tv, 0);
		mMilliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;

		actors.spliceTail(&mActiveActors);
		if (!actors.isEmpty())
			timeout = 0;
		else if(!mWaitingActors.isEmpty()) {
			timeout = mWaitingActors.first()->container()->nextTimeoutMilliseconds() - mMilliseconds;
			if (timeout < 0)
				timeout = 0;
		} else
			timeout = -1;

		n_events = epoll_wait(mEpollFd, events, MAX_EVENTS, timeout);
		for (int i = 0; i < n_events; i++) {
			Watch *watch = reinterpret_cast<Watch *>(events[i].data.ptr);
			/* TODO: set watch->readOrWrite */
			watch->invokeHandler();
		}

		for (ListItem<Actor> *item = mWaitingActors.first(); item; item = mWaitingActors.next(item)) {
			Actor *actor = item->container();

			if (actor->nextTimeoutMilliseconds() > mMilliseconds) {
				break;
			}

			bool alive = actor->handleTimeouts(mMilliseconds);
			if (alive) {
				if (!actor->nextTimeoutMilliseconds())
					mPassiveActors.addTail(&actor->listItem);
				else
					waitTimeout(actor);
			} else
				delete actor;

		}

		for (ListItem<Actor> *item = actors.first(); item; item = actors.next(item)) {
			Actor *actor = item->container();

			if (!actor->mQueuedMessages.isEmpty()) {
				sDispatchLock.lock();
				actor->mIncomingMessages.spliceTail(&actor->mQueuedMessages);
				sDispatchLock.unlock();
			}

			bool alive = actor->handleMessages(mMilliseconds);
			if (alive) {
				if (!actor->mIncomingMessages.isEmpty())
					mActiveActors.addTail(&actor->listItem);
				else if (!actor->nextTimeoutMilliseconds()) {
					actor->mActive = 0;
					mPassiveActors.addTail(&actor->listItem);
				} else {
					actor->mActive = 0;
					waitTimeout(actor);
				}
			} else
				delete actor;
		}
	}
}

void Dispatcher::queueMessage(Message *message)
{
	if (message->sender && message->sender->mDispatcher == message->receiver->mDispatcher) {
		message->receiver->mIncomingMessages.addTail(&message->listItem);
		activateActor(message->receiver);
	} else
		mOutgoingMessages.addTail(&message->listItem);
}

void Dispatcher::waitTimeout(Actor *actor)
{
	for (ListItem<Actor> *item = mWaitingActors.first(); item; item = mWaitingActors.next(item)) {
		if (item->container()->nextTimeoutMilliseconds() > actor->nextTimeoutMilliseconds()) {
			actor->listItem.addBefore(item);
			return;
		}
	}
	mWaitingActors.addTail(&actor->listItem);
}

}
