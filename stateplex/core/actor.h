/*
 * Stateplex - A server-side actor model library.
 *
 * core/actor.h
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

#ifndef INCLUDED_STATEPLEX_ACTOR_H
#define INCLUDED_STATEPLEX_ACTOR_H

#include "list.h"
#include "message.h"

namespace Stateplex {

class Dispatcher;
class Source;
class Timeout;

class Actor : public ListItem {
	friend class Dispatcher;

	Dispatcher *mDispatcher;
	
	List<Message> mQueuedMessages;
	List<Message> mIncomingMessages;
	List<Timeout> mTimeouts;
	
	unsigned int mAlive : 1;
	unsigned int mActive : 1;
	
	bool handleTimeouts(unsigned long milliseconds);
	bool handleMessages(unsigned long milliseconds);

protected:
	template<typename T, typename M> void queueMessage(M *message, Actor *sender, T *handlerObject, void (T::*handlerFunction)(M *message));

	void addTimeout(Timeout *timeout);
	template<typename T> Timeout *addTimeout(unsigned long milliseconds, T *callbackObject, void (T::*callbackFunction)(Timeout *timeout));
	void deleteTimeout(Timeout *timeout);
	unsigned long nextTimeoutMilliseconds();

	template<typename T> Source *addSource(int fd, T *callbackObject, void (T::*callbackFunction)(Source *source));
	void deleteWatch(Source *source);

	void setCallback(Message *message, void (Actor::*callback)(Message *message));

public:
	Actor(Dispatcher *dispatcher);
	virtual ~Actor();

	Dispatcher *dispatcher();
	bool isAlive();
	bool isActive(unsigned long milliseconds);
};

}

/*** Inline implementations ***/

#include "message.h"
#include "dispatcher.h"
#include "timeout.h"

namespace Stateplex {

/**
 * Constructor that initialize variables. 
 * Function that activates this actor. 
 * @param *dispatcher 		is a pointer to Dispatcher.
 */

inline Actor::Actor(Dispatcher *dispatcher)
	: mAlive(1), mActive(0), mDispatcher(dispatcher)
{
	dispatcher->activateActor(this);
}

/**
 * Destructor for class Actor.
 */

inline Actor::~Actor()
{ }

/**
 * A template function thats swithces values between variables.
 * Sends message, queue message and receive message and sets the handler of the message.
 * @param *message        is pointer to message.
 * @param *sender         is a pointer to sender.
 */

template<typename T, typename M>
inline void Actor::queueMessage(M *message, Actor *sender, T *handlerObject, void (T::*handlerFunction)(M *message))
{
	message->sender = sender;
	message->receiver = this;
	message->handler.set(handlerObject, handlerFunction);
	mDispatcher->queueMessage(message);
}

/**
 * Function that return value to pointer dispatcher.
 *
 * @return mDispatcher	actors dispatcher.
 */

inline Dispatcher *Actor::dispatcher()
{
	return mDispatcher;
}

/**
 * Function that returns time out in milliseconds if timeout is set otherwise zero.
 *
 */

inline unsigned long Actor::nextTimeoutMilliseconds()
{
	Timeout *timeout = mTimeouts.first();
	return timeout ? timeout->milliseconds() : 0;
}

/**
 * Function that pass values other than pointer Alive.
 *
 * @return 	mAlive integer value.
 */

inline bool Actor::isAlive()
{
	return !!mAlive;
}

/**
 * Function that checks if the actor is active.
 *
 * @return 	true if active, else false.
 */

inline bool Actor::isActive(unsigned long milliseconds)
{
	return true; /* TODO */
}

/**
 * A template function that adds new timeout to actor.
 *
 * @param millisecond	value of the timeout.
 * @param *object	handler object where the timeout is added.
 * @param *callback	callback function.
 */

template<typename T>
Timeout *Actor::addTimeout(unsigned long milliseconds, T *object, void (T::*callback)(Timeout *timeout))
{
	Timeout *timeout = new Timeout(milliseconds, object, callback);
	addTimeout(timeout);
}

/**
 * A function that removes a timeout that has been pointed.
 * after removing the timeout it deletes the pointer.
 * 
 * @param timeout	timeout to be removed.
 */

inline void Actor::deleteTimeout(Timeout* timeout)
{
	timeout->remove();
	delete timeout;
}

}

#endif
