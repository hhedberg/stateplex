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

/** 
 * @brief Class Source.
 *
 * Class Source is source.
 */

class Source : public ListItem {
	friend class Dispatcher;

	Actor *mActor;
	int mFd;
	bool mEnabled;

protected:
	Source(Actor *actor, int fd, bool enabled = true);

	int fd() const;

	virtual void handleReady(bool readyToReady, bool readyToWrite) = 0;
	void setFd(int fd);

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
	
/** 
 * Constructor for class Source.
 *
 * @param *actor	actor that uses this source.
 * @param fd		file descriptor.
 * @param enabled	this source will be added to dispatcher if true.
 */

inline Source::Source(Actor *actor, int fd, bool enabled)
	: mActor(actor), mFd(fd), mEnabled(enabled)
{
	if (enabled)
		mActor->dispatcher()->addSource(this);
}

/** 
 * Destructor for class Source.
 */
 
inline Source::~Source()
{ }

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
 * Sets parameter as file descriptor.
 *
 * @param fd	file descriptor to be set.
 */
 
inline void Source::setFd(int fd)
{
	mFd = fd;
}

/** 
 * Returns the address of the actor that uses this source.
 *
 * @return		pointer to actor to be returned.
 */
 
inline Actor *Source::actor() const
{
	return mActor;
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
}

}

#endif
