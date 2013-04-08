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

class CallbackMessage : public Message {
	Method mCallback;

protected:
	template<typename T> CallbackMessage(Actor *sender, Actor *receiver, T *callbackObject, void (T::*callbackFunction)(CallbackMessage *));

	virtual void handle(Actor *sender, Actor *receiver) = 0;
	void invokeCallback() const;

public:
	virtual ~CallbackMessage();
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename T>
inline CallbackMessage::CallbackMessage(Actor *sender, Actor *receiver, T *callbackObject, void (T::*callbackFunction)(CallbackMessage *))
	: Message(sender, receiver), mCallback(callbackObject, callbackFunction)
{ }

inline CallbackMessage::~CallbackMessage()
{ }

inline void CallbackMessage::invokeCallback() const
{
	/* TODO */
}

}

#endif
