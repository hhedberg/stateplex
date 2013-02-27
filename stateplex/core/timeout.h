/*
 * Stateplex - A server-side actor model library.
 *
 * core/timeout.h
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

#ifndef INCLUDED_STATEPLEX_TIMEOUT_H
#define INCLUDED_STATEPLEX_TIMEOUT_H

#include "list.h"
#include "method.h"

namespace Stateplex {

class Timeout : public ListItem {
	friend class Actor;

	unsigned long mMilliseconds;
	Method mHandler;

	void invokeHandler();

public:
	template<typename T> Timeout(unsigned long milliseconds, T *handlerObject, void (T::*handlerFunction)(Timeout *timeout));

	unsigned long milliseconds() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Watch *watch));
};

}

/*** Template implementations ***/

namespace Stateplex {

template<typename T>
Timeout::Timeout(unsigned long milliseconds, T *handlerObject, void (T::*handlerFunction)(Timeout *timeout))
	: mMilliseconds(milliseconds), mHandler(handlerObject, handlerFunction)
{ }

template<typename T>
inline void Timeout::setHandler(T *handlerObject, void (T::*handlerFunction)(Watch *watch))
{
	mHandler.set(handlerObject, handlerFunction);
}

}

/*** Inline implementations ***/

namespace Stateplex {

inline void Timeout::invokeHandler()
{
	mHandler.invoke(this);
}

inline unsigned long Timeout::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
