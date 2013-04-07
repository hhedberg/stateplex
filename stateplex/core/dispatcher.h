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

/** 
 * @brief Handles message passing between different actors.
 */

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

/**
 * Default destructor for dispatcher.
 */
 
inline Dispatcher::~Dispatcher()
{ }

/**
 * Function that activates an actor if not already activated and
 * adds it to the list of activated actors.
 *
 * @param *actor        pointer to actor that is the target.
 */
 
inline void Dispatcher::activateActor(Actor *actor)
{
	if (!actor->mActive) {
		mActiveActors.addTail(actor);
		actor->mActive = 1;
	}
}

/**
 * Function that adds or updates source.
 *
 * @param *source        target source to add or update.
 * @param epollOperation command for epoll.										
 */

inline void Dispatcher::addOrUpdateSource(Source *source, int epollOperation)
{
	struct ::epoll_event event;

	event.events = (source->mReadable ? EPOLLIN : 0 ) | (source->mWritable ? EPOLLOUT : 0) | EPOLLET;
	event.data.ptr = source;
	epoll_ctl(mEpollFd, epollOperation, source->mFd, &event);
}

/**
 * Function that adds source. 
 *
 * @param *source	target source to add.
 */
 
inline void Dispatcher::addSource(Source *source)
{
	addOrUpdateSource(source, EPOLL_CTL_ADD);
}

/**
 * Function that updates the source.
 *
 * @param *source        target source to update.
 */
 
inline void Dispatcher::updateSource(Source *source)
{
	addOrUpdateSource(source, EPOLL_CTL_MOD);
}

/**
 * Function that removes the source.
 *
 * @param *source        target source to remove.
 */
 
inline void Dispatcher::removeSource(Source *source)
{
	epoll_ctl(mEpollFd, EPOLL_CTL_DEL, source->mFd, 0);
}

/**
 * Function that returns milliseconds.
 *
 * @return        milliseconds
 */
 
inline unsigned long Dispatcher::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
