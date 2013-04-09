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
template<typename T> class Message;
class Source;
class Allocator;

/** 
 * @brief Handles message passing between different actors.
 */

class Dispatcher {
	static Spinlock sDispatchLock;
	static Allocator *sRecycledAllocator;
	static __thread Dispatcher *sCurrentDispatcher;

	int mEpollFd;
	List<Actor> mActiveActors;
	List<Actor> mWaitingActors;
	List<Actor> mPassiveActors;
	List<Message<Actor> > mOutgoingMessages;
	Allocator *mAllocator;
	
	bool mRunning;
	unsigned long mMilliseconds;

	void activateActor(Actor *actor);
	void allocateMemory();
	void addOrUpdateSource(Source *source, int epollOperation);
	void waitTimeout(Actor *actor);
	
public:
	Dispatcher();
	~Dispatcher();
	
	void run();
	Allocator *allocator() const;
	void addSource(Source *source);
	void updateSource(Source *source);
	void removeSource(Source *source);
	template<typename T> void queueMessage(Message<T> *message);
	unsigned long milliseconds() const;

	static Dispatcher *leastLoaded();
	static Dispatcher *current();
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

inline Allocator *Dispatcher::allocator() const
{
	return mAllocator;
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
 * Queues the given message to be handled by the receiving actor.
 */

template<typename T>
void Dispatcher::queueMessage(Message<T> *message)
{
	Message<Actor> *m = reinterpret_cast<Message<Actor> *>(message);
	if (message->actor() && message->actor()->mDispatcher == message->receiver()->mDispatcher) {
		message->receiver()->mIncomingMessages.addTail(m);
		activateActor(message->mReceiver);
	} else
		mOutgoingMessages.addTail(m);
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

inline Dispatcher *Dispatcher::current()
{
	return sCurrentDispatcher;
}

}

#endif
