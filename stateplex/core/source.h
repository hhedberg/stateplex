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

class Actor;

class Source : public ListItem {
	friend class Dispatcher;

	Actor *mActor;
	int mFd;
	Method mHandler;

protected:
	template<typename T> Source(Actor *actor, T *handlerObject, void (T::*handlerFunction)(Source *source));

	virtual void handleReady();
	void invokeHandler();
	void setFd(int fd);

public:
	template<typename T> Source(Actor *actor, int fd, T *handlerObject, void (T::*handlerFunction)(Source *source));
	virtual ~Source();

	int fd() const;
	Actor *actor() const;
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(Source *source));
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename T>
Source::Source(Actor *actor, T *handlerObject, void (T::*handlerFunction)(Source *source))
	: mActor(actor), mFd(-1), mHandler(handlerObject, handlerFunction)
{ }

template<typename T>
Source::Source(Actor *actor, int fd, T *handlerObject, void (T::*handlerFunction)(Source *source))
	: mActor(actor), mFd(fd), mHandler(handlerObject, handlerFunction)
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

inline int Source::fd() const
{
	return mFd;
}

inline Actor *Source::actor() const
{
	return mActor;
}

template<typename T>
void Source::setHandler(T *handlerObject, void (T::*handlerFunction)(Source *source))
{
	mHandler.set(handlerObject, handlerFunction);
}

}

#endif
