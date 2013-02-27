/*
 * Stateplex - A server-side actor model library.
 *
 * core/dispatcher.h
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

#ifndef INCLUDED_STATEPLEX_DISPATCHER_H
#define INCLUDED_STATEPLEX_DISPATCHER_H

#include "spinlock.h"
#include "list.h"

namespace Stateplex {

class Actor;
class Message;
class Watch;

class Dispatcher {
	static Spinlock sDispatchLock;

	int mEpollFd;
	List<Actor> mActiveActors;
	List<Actor> mWaitingActors;
	List<Actor> mPassiveActors;
	List<Message> mOutgoingMessages;
	
	bool mRunning;
	unsigned long mMilliseconds;

	void allocateMemory();
	void waitTimeout(Actor *actor);
	
public:
	Dispatcher();
	~Dispatcher();
	
	void run();
	void addWatch(Watch *watch);
	void deleteWatch(Watch *watch);
	void activateActor(Actor *actor);
	void queueMessage(Message *message);
	unsigned long milliseconds() const;

	static Dispatcher *leastLoaded();
};

}

/*** Inline implementations ***/

#include <sys/epoll.h>

#include "actor.h"
#include "watch.h"

namespace Stateplex {

inline Dispatcher::Dispatcher()
	: mRunning(true), mMilliseconds(0), mEpollFd(0)
{ }

inline Dispatcher::~Dispatcher()
{ }

inline void Dispatcher::activateActor(Actor *actor)
{
	if (!actor->mActive) {
		mActiveActors.addTail(actor);
		actor->mActive = 1;
	}
}

inline void Dispatcher::addWatch(Watch *watch)
{
	struct ::epoll_event event;

	event.events = EPOLLIN | EPOLLOUT;
	event.data.ptr = watch;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, watch->mFd, &event);
}

inline void Dispatcher::deleteWatch(Watch *watch)
{
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, watch->mFd, 0);
	delete watch;
}

inline unsigned long Dispatcher::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
