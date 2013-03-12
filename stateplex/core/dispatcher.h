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
	void addOrUpdateSource(Source *source, int epollOperation);
	void waitTimeout(Actor *actor);
	
public:
	Dispatcher();
	~Dispatcher();
	
	void run();
	void addSource(Source *source);
	void updateSource(Source *source);
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

inline Dispatcher::~Dispatcher()
{ }

inline void Dispatcher::activateActor(Actor *actor)
{
	if (!actor->mActive) {
		mActiveActors.addTail(actor);
		actor->mActive = 1;
	}
}

inline void Dispatcher::addOrUpdateSource(Source *source, int epollOperation)
{
	struct ::epoll_event event;

	event.events = (source->mReadable ? EPOLLIN : 0 ) | (source->mWritable ? EPOLLOUT : 0) | EPOLLET;
	event.data.ptr = source;
	epoll_ctl(mEpollFd, epollOperation, source->mFd, &event);
}

inline void Dispatcher::addSource(Source *source)
{
	addOrUpdateSource(source, EPOLL_CTL_ADD);
}

inline void Dispatcher::updateSource(Source *source)
{
	addOrUpdateSource(source, EPOLL_CTL_MOD);
}

inline void Dispatcher::removeSource(Source *source)
{
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, source->mFd, 0);
}

inline unsigned long Dispatcher::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
