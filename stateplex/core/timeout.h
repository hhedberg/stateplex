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

/**
 * Class Timeout.
 *
 * Timeouts are used to control actors behavior in a way
 * that it can be given a timeout to not act for a specific time.
 */

class Timeout : public ListItem {
	friend class Actor;

	unsigned long mMilliseconds;
	Method mHandler;

	void invokeHandler();

public:
	template<typename T> Timeout(unsigned long milliseconds, T *handlerObject, void (T::*handlerFunction)(Timeout *timeout));

	unsigned long milliseconds() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Timeout *timeout));
};

}

/*** Template implementations ***/

namespace Stateplex {

/**
 * Constructor for class Timeout.
 * The constructor initializes a new instance of class bitset.
 *
 * @param milliseconds		time in milliseconds for the timeout object.
 * @param *handlerObject	pointer to the object that handles this timeout.
 * @param *handlerFunction	pointer to the handler objects member function.
 */

template<typename T>
Timeout::Timeout(unsigned long milliseconds, T *handlerObject, void (T::*handlerFunction)(Timeout *timeout))
	: mMilliseconds(milliseconds), mHandler(handlerObject, handlerFunction)
{ }

/**
 * Function that sets the timeout's handler and function to the pointed one.
 *
 * @param *handlerObject	pointer to the handler of the timeout.
 * @param *handlerFunction	pointer to the member function of the handlers.
 */

template<typename T>
inline void Timeout::setHandler(T *handlerObject, void (T::*handlerFunction)(Timeout *timeout))
{
	mHandler.set(handlerObject, handlerFunction);
}

}

/*** Inline implementations ***/

namespace Stateplex {

/**
 * Function that calls timeout's method handler and gives itself as argument.
 * timeout is then used to get the handler functions object and member function
 * and after that the function is executed.
 */

inline void Timeout::invokeHandler()
{
	mHandler.invoke(this);
}

/**
 * Function that returns the timeouts milliseconds.
 *
 * @return milliseconds of the timeout.
 */

inline unsigned long Timeout::milliseconds() const
{
	return mMilliseconds;
}

}

#endif
