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
	int mEnabled : 1;
	int mHandled : 1;
	int mDispatched : 1;
	int mReadable : 1;
	int mWritable : 1;

	void manageDispatching();

protected:
	Source(Actor *actor, int fd, bool readable, bool writable, bool enabled = true, bool handled = false);

	int fd() const;
	bool isReadable() const;
	bool isWritable() const;

	virtual void handleReady(bool readyToRead, bool readyToWrite) = 0;
	void setFd(int fd);
	void setHandled(bool handled);
	void setMode(bool readable, bool writable);

public:
	virtual ~Source();

	Actor *actor() const;
	bool isOpen() const;
	bool isEnabled() const;

	void close();
	void setEnabled(bool enabled);
};

}

/*** Inline implementations ***/

#include <unistd.h>

#include "actor.h"

namespace Stateplex {

inline Source::Source(Actor *actor, int fd, bool readable, bool writable, bool enabled, bool handled)
	: mActor(actor), mFd(fd), mReadable(readable), mWritable(writable), mEnabled(enabled), mHandled(handled), mDispatched(0)
{
	manageDispatching();
}

inline Source::~Source()
{ }

inline int Source::fd() const
{
	return mFd;
}

inline void Source::setFd(int fd)
{
	mFd = fd;
	manageDispatching();
}

inline void Source::setHandled(bool handled)
{
	mHandled = handled;
	manageDispatching();
}

inline void Source::setMode(bool readable, bool writable)
{
	if (mReadable == readable && mWritable == writable)
		return;

	mReadable = readable;
	mWritable = writable;
	if (mDispatched)
		actor()->dispatcher()->updateSource(this);
}

inline Actor *Source::actor() const
{
	return mActor;
}

inline bool Source::isOpen() const
{
	return (mFd != -1 ? true : false);
}

inline bool Source::isEnabled() const
{
	return !!mEnabled;
}

inline void Source::close()
{
	::close(mFd);
	mFd = -1;
	manageDispatching();
}

inline void Source::setEnabled(bool enabled)
{
	mEnabled = enabled;
	manageDispatching();
}

}

#endif
