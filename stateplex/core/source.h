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

protected:
	template<typename T> Source(T *handlerObject, void (T::*handlerFunction)(Source *source));

	virtual void handleReady();
	void invokeHandler();
	void setFd(int fd);

public:
	template<typename T> Source(int fd, T *handlerObject, void (T::*handlerFunction)(Source *source));
	virtual ~Source();

	int fd() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Source *source));
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename T> Source::Source(T *handlerObject, void (T::*handlerFunction)(Source *source))
	: mFd(-1), mHandler(handlerObject, handlerFunction)
{ }

template<typename T> Source::Source(int fd, T *handlerObject, void (T::*handlerFunction)(Source *source))
	: mFd(fd), mHandler(handlerObject, handlerFunction)
{ }

inline Source::~Source()
{ }

inline void Source::invokeHandler()
{
	mHandler.invoke(this);
}

inline void Source::setFd(int fd)
{
	mFd = fd;
}

}

#endif
