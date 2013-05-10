/*
 * Stateplex - A server-side actor model library.
 *
 * core/callbackmessage.h
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

#ifndef INCLUDED_STATEPLEX_CALLBACK_MESSAGE_H
#define INCLUDED_STATEPLEX_CALLBACK_MESSAGE_H

#include "method.h"

namespace Stateplex {

template<typename Receiver, typename Subtype>
class CallbackMessage : public Message<Receiver> {
	Method mCallback;

protected:
	template<typename Sender> CallbackMessage(Sender *sender, Receiver *receiver, void (Sender::*callbackFunction)(Subtype *));

	virtual void handle(Actor *sender, Receiver *receiver) = 0;
	void invokeCallback(Subtype * message) const;

public:
	virtual ~CallbackMessage();
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename Receiver, typename Subtype> template<typename Sender>
inline CallbackMessage<Receiver, Subtype>::CallbackMessage(Sender *sender, Receiver *receiver, void (Sender::*callbackFunction)(Subtype *))
	: Message<Receiver>(sender, receiver), mCallback(sender, callbackFunction)
{ }

template<typename Receiver, typename Subtype>
inline CallbackMessage<Receiver, Subtype>::~CallbackMessage()
{ }

template<typename Receiver, typename Subtype>
inline void CallbackMessage<Receiver, Subtype>::invokeCallback(Subtype *message) const
{
	/* TODO */
	mCallback.invoke(message);
}

}

#endif
