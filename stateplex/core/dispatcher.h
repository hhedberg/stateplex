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
class Source;

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
	void addSource(Source *source);
	void removeSource(Source *source);
	void activateActor(Actor *actor);
	void queueMessage(Message *message);
	unsigned long milliseconds() const;

	static Dispatcher *leastLoaded();
};

}

/*** Inline implementations ***/

#include <sys/epoll.h>

#include "actor.h"
#include "source.h"

namespace Stateplex {

/**
 * Constructor that initialize variables. 
 *
 */

inline Dispatcher::Dispatcher()
	: mRunning(true), mMilliseconds(0), mEpollFd(0)
{ }

/**
 * Distructor function.
 *
 */

inline Dispatcher::~Dispatcher()
{ }

/**
 * Function that activate an actor if active is 1.
 *
 * @param *actor        pointer to actor that is used for activating actors.
 */

inline void Dispatcher::activateActor(Actor *actor)
{
	if (!actor->mActive) {
		mActiveActors.addTail(actor);
		actor->mActive = 1;
	}
}

/**
 * Function that controls interface for an epoll discriptor. 
 *
 */

inline void Dispatcher::addSource(Source *source)
{
	struct ::epoll_event event;

	event.events = EPOLLIN | EPOLLOUT | EPOLLET;
	event.data.ptr = source;
	epoll_ctl(mEpollFd, EPOLL_CTL_ADD, source->mFd, &event);
}

/**
 * Function that controls the values to be zero otherwise removes.
 *
 * @param EPOLL_CTL_DEL        removes the target file discriptor.
 */

inline void Dispatcher::removeSource(Source *source)
{
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, source->mFd, 0);
}

/**
 * Function that return time in milliseconds.
 *
 * @return        milliseconds is int long value.
 */

inline unsigned long Dispatcher::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
