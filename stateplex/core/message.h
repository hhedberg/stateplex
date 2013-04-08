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

class Message : public Object, public ListItem {
	friend class Dispatcher;
	friend class Actor;
	
	Actor *mReceiver;

protected:
	Message(Actor *sender, Actor *receiver);

	virtual void handle(Actor *sender, Actor *receiver) = 0;

public:
	virtual ~Message();
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline Message::Message(Actor *sender, Actor *receiver)
	: Object(sender), mReceiver(receiver)
{ }

inline Message::~Message()
{ }

}

#endif
