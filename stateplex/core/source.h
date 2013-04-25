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

#include "object.h"
#include "list.h"
#include "method.h"

namespace Stateplex {

class Actor;

/** 
 * @brief Inherited from ListItem. Source is used by the net module.
 */

class Source : public virtual Object, public ListItem {
	friend class Dispatcher;

	int mFd;
	int mEnabled : 1;
	int mDispatched : 1;
	int mReadable : 1;
	int mWritable : 1;

	void manageDispatching();
	void setNonblocking();

protected:
	Source(Actor *actor, int fd, bool readable, bool writable, bool enabled = true, bool handled = false);

	int fd() const;
	bool isReadable() const;
	bool isWritable() const;

	virtual void handleReady(bool readyToRead, bool readyToWrite) = 0;
	void setFd(int fd);
	void setMode(bool readable, bool writable);

public:
	virtual ~Source();

	bool isOpen() const;
	bool isEnabled() const;

	void close();
	void setEnabled(bool enabled);
};

}

/*** Inline implementations ***/

#include <unistd.h>
#include <fcntl.h>

#include "actor.h"

namespace Stateplex {
	
/** 
 * Constructor for class Source.
 *
 * @param *actor	pointer to the actor that uses this source.
 * @param fd		file descriptor.
 * @param enabled	this source will be added to dispatcher if true.
 */

inline Source::Source(Actor *actor, int fd, bool readable, bool writable, bool enabled, bool handled)
	: Object(actor), mFd(fd), mReadable(readable), mWritable(writable), mEnabled(enabled), mDispatched(0)
{
	if (mFd != -1)
		setNonblocking();
	manageDispatching();
}

/** 
 * Destructor for class Source.
 */
 
inline Source::~Source()
{ }

inline void Source::setNonblocking()
{
	int flags;

	if ((flags = fcntl(mFd, F_GETFL)) == -1 ||
		    fcntl(mFd, F_SETFL, flags | O_NONBLOCK) == -1)
		abort();
}

/** 
 * Returns file descriptor.
 *
 * @return		file descriptor to be returned.
 */
 
inline int Source::fd() const
{
	return mFd;
}

/** 
 * Sets parameter as file descriptor and check the source that should it be given to dispatcher.
 *
 * @param fd		file descriptor to be set.
 */
 
inline void Source::setFd(int fd)
{
	if (mFd != -1 || fd == -1)
		abort();

	mFd = fd;
	setNonblocking();
	manageDispatching();
}

/** 
 * Checks if file descriptor is open.
 *
 * @return		true if open, otherwise false.
 */
 
inline bool Source::isOpen() const
{
	return (mFd != -1 ? true : false);
}

/** 
 * Checks if source is enabled.
 *
 * @return		true if enabled, otherwise false.
 */
 
inline bool Source::isEnabled() const
{
	return !!mEnabled;
}

/** 
 * Closes file descriptor.
 */
 
inline void Source::close()
{
	::close(mFd);
	mFd = -1;
	manageDispatching();
}

/**
 * Function that sets source enabled.
 * Calls for manageDispatching function to check the source
 * for modifications.
 *
 * @param enabled	boolean value to set.
 */

inline void Source::setEnabled(bool enabled)
{
	mEnabled = enabled;
	manageDispatching();
}

}

#endif
