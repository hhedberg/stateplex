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
		/* Dispatch outgoing messages with the lock hold */
		if (!mOutgoingMessages.isEmpty()) {
			sDispatchLock.lock();
			for (ListIterator<Message> iterator(&mOutgoingMessages); iterator.hasCurrent(); iterator.subsequent()) {
				Message *message = iterator.current();
				Actor *receiver = message->receiver;
				receiver->mQueuedMessages.addTail(message);
				activateActor(receiver);
			}
			sDispatchLock.unlock();
		}

		/* Start the new cycle */
		gettimeofday(&tv, 0);
		mMilliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;

		/* Get the active actors, check when the next waiting actor timeouts, or wait indefinitely */
		actors.spliceTail(&mActiveActors);
		if (!actors.isEmpty())
			timeout = 0;
		else if(!mWaitingActors.isEmpty()) {
			timeout = mWaitingActors.first()->nextTimeoutMilliseconds() - mMilliseconds;
			if (timeout < 0)
				timeout = 0;
		} else
			timeout = -1;

		/* Receive external events i.e. poll watches */
		n_events = epoll_wait(mEpollFd, events, MAX_EVENTS, timeout);
		for (int i = 0; i < n_events; i++) {
			Source *source = reinterpret_cast<Source *>(events[i].data.ptr);
			/* TODO: set source->readOrWrite */
			source->invokeHandler();
		}

		/* Handle timeouts for waiting actors */
		for (ListIterator<Actor> iterator(&mWaitingActors); iterator.hasCurrent(); iterator.subsequent()) {
			Actor *actor = iterator.current();

			if (actor->nextTimeoutMilliseconds() > mMilliseconds) {
				break;
			}

			bool alive = actor->handleTimeouts(mMilliseconds);
			if (alive) {
				if (!actor->nextTimeoutMilliseconds())
					mPassiveActors.addTail(actor);
				else
					waitTimeout(actor);
			} else
				delete actor;

		}

		/* Handle active actors i.e. actors that have incoming messages */
		for (ListIterator<Actor> iterator(&actors); iterator.hasCurrent(); iterator.subsequent()) {
			Actor *actor = iterator.current();

			/* Dispatch incoming messges for this actor with the lock hold */
			if (!actor->mQueuedMessages.isEmpty()) {
				sDispatchLock.lock();
				actor->mIncomingMessages.spliceTail(&actor->mQueuedMessages);
				sDispatchLock.unlock();
			}

			bool alive = actor->handleMessages(mMilliseconds);
			if (alive) {
				if (!actor->mIncomingMessages.isEmpty())
					mActiveActors.addTail(actor);
				else if (!actor->nextTimeoutMilliseconds()) {
					actor->mActive = 0;
					mPassiveActors.addTail(actor);
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
		message->receiver->mIncomingMessages.addTail(message);
		activateActor(message->receiver);
	} else
		mOutgoingMessages.addTail(message);
}

void Dispatcher::waitTimeout(Actor *actor)
{
	for (ListIterator<Actor> iterator(&mWaitingActors); iterator.hasCurrent(); iterator.subsequent()) {
		Actor *existing = iterator.current();
		if (actor->nextTimeoutMilliseconds() < existing->nextTimeoutMilliseconds()) {
			actor->addBefore(existing);
			return;
		}
	}
	mWaitingActors.addTail(actor);
}

}
