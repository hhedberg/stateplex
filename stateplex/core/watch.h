/*
 * Stateplex - A server-side actor model library.
 *
 * core/watch.h
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

#ifndef INCLUDED_STATEPLEX_WATCH_H
#define INCLUDED_STATEPLEX_WATCH_H

#include "list.h"
#include "method.h"

namespace Stateplex {

class Watch : public ListItem {
	friend class Dispatcher;

	int mFd;
	Method mHandler;

	void invokeHandler();

public:
	template<typename T> Watch(int fd, T *handlerObject, void (T::*handlerFunction)(Watch *watch));

	int fd() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Watch *watch));
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline void Watch::invokeHandler()
{
	mHandler.invoke(this);
}

}

#endif
