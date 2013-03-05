/*
 * Stateplex - A server-side actor model library.
 *
 * core/source.h
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

#ifndef INCLUDED_STATEPLEX_SOURCE_H
#define INCLUDED_STATEPLEX_SOURCE_H

#include "list.h"
#include "method.h"

namespace Stateplex {

class Source : public ListItem {
	friend class Dispatcher;

	int mFd;
	Method mHandler;

	void invokeHandler();

public:
	template<typename T> Source(int fd, T *handlerObject, void (T::*handlerFunction)(Source *watch));

	int fd() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Source *watch));
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline void Source::invokeHandler()
{
	mHandler.invoke(this);
}

}

#endif
