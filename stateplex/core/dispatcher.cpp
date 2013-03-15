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
#include <stdlib.h>
#include <stdio.h>

#include "dispatcher.h"
#include "actor.h"

/* TODO: Parametrise this */
#define MAX_EVENTS 10

namespace Stateplex {

Spinlock Dispatcher::sDispatchLock;

Dispatcher::Dispatcher()
	: mRunning(true), mMilliseconds(0)
{
	mEpollFd = epoll_create(1024);
}

/**
 * Function that dispatch incoming and outgoing messages with the lock hold,handles time out for active actors. 
 * Dispatch outgoing messages with the lock hold
 * Dispatch incoming messages with the lock hold
 * Release the lock if it was acquired in dispatching
 * Handle timeouts for waiting actors
 * Handle active actors i.e. actors that have incoming messages
 */

void Dispatcher::run()
{
	bool locked;
	struct epoll_event events[MAX_EVENTS];
	int n_events;
	struct timeval tv;
	List<Actor> actors;
	int timeout;
	
	while (mRunning) {
		locked = false;
		actors.spliceTail(&mActiveActors);

		/* Dispatch outgoing messages with the lock hold */
		if (!mOutgoingMessages.isEmpty()) {
			sDispatchLock.lock();
			locked = true;
			for (ListIterator<Message> iterator(&mOutgoingMessages); iterator.hasCurrent(); iterator.subsequent()) {
				Message *message = iterator.current();
				Actor *receiver = message->receiver;
				receiver->mQueuedMessages.addTail(message);
				activateActor(receiver);
			}
		}

		/* Dispatch incoming messages with the lock hold */
		if (!actors.isEmpty()) {
			if (!locked) {
				sDispatchLock.lock();
				locked = true;
			}
			for (ListIterator<Actor> iterator(&actors); iterator.hasCurrent(); iterator.subsequent()) {
				Actor *actor = iterator.current();
				actor->mIncomingMessages.spliceTail(&actor->mQueuedMessages);
			}
		}

		/* Release the lock if it was acquired in dispatching */
		if (locked)
			sDispatchLock.unlock();

		/* Start a new cycle */
		gettimeofday(&tv, 0);
		mMilliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;

		/* Do not block, if there are active actors, check when the next waiting actor timeouts, or wait indefinitely */
		if (!actors.isEmpty())
			timeout = 0;
		else if(!mWaitingActors.isEmpty()) {
			timeout = mWaitingActors.first()->nextTimeoutMilliseconds() - mMilliseconds;
			if (timeout < 0)
				timeout = 0;
		} else
			timeout = -1;

		/* Receive external events i.e. poll sources */
		n_events = epoll_wait(mEpollFd, events, MAX_EVENTS, timeout);
		if (n_events == -1) {
			perror("epoll_wait");
			abort();
		}
		for (int i = 0; i < n_events; i++) {
			Source *source = reinterpret_cast<Source *>(events[i].data.ptr);
			source->handleReady(events[i].events & EPOLLIN, events[i].events & EPOLLOUT);
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

/**
 * Function that queue message, activate receiver actor otherwise add to outgoing messages. 
 *
 */

void Dispatcher::queueMessage(Message *message)
{
	if (message->sender && message->sender->mDispatcher == message->receiver->mDispatcher) {
		message->receiver->mIncomingMessages.addTail(message);
		activateActor(message->receiver);
	} else
		mOutgoingMessages.addTail(message);
}

/**
 * Function that handles time out for waiting actors. 
 *
 */

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
