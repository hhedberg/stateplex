/*
 * Stateplex - A server-side actor model library.
 *
 * core/message.h
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

#ifndef INCLUDED_STATEPLEX_MESSAGE_H
#define INCLUDED_STATEPLEX_MESSAGE_H

#include "list.h"
#include "method.h"

namespace Stateplex {

class Dispatcher;
class Actor;

/** 
 * @brief Inherited from ListItem. It is used by actors to 
 * send messages between them.
 */

template<typename Receiver>
class Message : public Object, public ListItem {
	friend class Dispatcher;
	friend class Actor;
	
	Receiver *mReceiver;

protected:
	Message(Actor *sender, Receiver *receiver);

	virtual void handle(Actor *sender, Receiver *receiver) = 0;

public:
	virtual ~Message();

	Actor *receiver() const;
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename Receiver>
inline Message<Receiver>::Message(Actor *sender, Receiver *receiver)
	: Object(sender), mReceiver(receiver)
{ }

template<typename Receiver>
inline Message<Receiver>::~Message()
{ }

template<typename Receiver>
inline Actor *Message<Receiver>::receiver() const
{
	return static_cast<Actor *>(mReceiver);
}

}

#endif
