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
class Watch;
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

	template<typename T> Watch *addWatch(int fd, T *callbackObject, void (T::*callbackFunction)(Watch *watch));
	void deleteWatch(Watch *watch);

	Dispatcher *dispatcher();
	void setCallback(Message *message, void (Actor::*callback)(Message *message));

public:
	Actor(Dispatcher *dispatcher);
	virtual ~Actor();

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
 * Constructor that initialize variable Alive, Active and Dispacther
 *
 * @param (*dispatcher)	is a pointer to Dispatcher
 */

inline Actor::Actor(Dispatcher *dispatcher)
	: mAlive(1), mActive(0), mDispatcher(dispatcher)
{
	dispatcher->activateActor(this);
}

/**
 *
 * Distructor
 *
 */

inline Actor::~Actor()
{ }

/**
 * A template function thats swithces values between variables
 * Sends message, queue message and receive message
 * @param *message        is pointer ro message
 * @param *sender         is a pointer to sender
 * @param *handlerObject        is a pointer to handlerObject
 * @param *handlerFunction         is a pointer to handlerFunction
 */

template<typename T, typename M>
inline void Actor::queueMessage(M *message, Actor *sender, T *handlerObject, void (T::*handlerFunction)(M *message))
{
	message->message.sender = sender;
	message->message.receiver = this;
	message->message.handler.set(handlerObject, handlerFunction);
	mDispatcher->queueMessage(&message->message);
}

/**
 * Function that return value to pointer dispatcher
 *
 *
 */

inline Dispatcher *Actor::dispatcher()
{
	return mDispatcher;
}

/**
 * Function that returns time out in milliseconds if timeout is set otherwise zero
 *
 *
 */

inline unsigned long Actor::nextTimeoutMilliseconds()
{
	Timeout *timeout = mTimeouts.first();
	return timeout ? timeout->milliseconds() : 0;
}
/**
 *
 *
 * @return 	mAlive integer value
 */
inline bool Actor::isAlive()
{
	return !!mAlive;
}

/**
 * Fucntion that return true 
 *
 * @return 	true
 */

inline bool Actor::isActive(unsigned long milliseconds)
{
	return true; /* TODO */
}

/**
 * A template function that creates new time out and add it to timeout function
 *
 *
 */

template<typename T>
Timeout *Actor::addTimeout(unsigned long milliseconds, T *object, void (T::*callback)(Timeout *timeout))
{
	Timeout *timeout = new Timeout(milliseconds, object, callback);
	addTimeout(timeout);
}

/**
 * A function that delete the timeout
 * 
 *
 */

inline void Actor::deleteTimeout(Timeout* timeout)
{
	timeout->remove();
	delete timeout;
}

}

#endif
