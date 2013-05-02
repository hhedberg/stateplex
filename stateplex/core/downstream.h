/*
 * Stateplex - A server-side actor model library.
 *
 * core/downstream.h
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

#ifndef INCLUDED_STATEPLEX_DOWNSTREAM_H
#define INCLUDED_STATEPLEX_DOWNSTREAM_H

#include "object.h"
#include "buffer.h"

namespace Stateplex {

/** 
 * @brief Used to set the downstream.
 */

class Upstream;

class Downstream : public virtual Object {
	friend class Upstream;

	Upstream *mUpstream;

protected:
	Downstream(Actor *actor);

	virtual void receiveDrainedFromUpstream() = 0;
	virtual void receiveFromUpstream(const char *data, Size length) = 0;
	virtual void receiveFromUpstream(Buffer<> *buffer) = 0;

public:
	virtual ~Downstream();

	void setUpstream(Upstream *upstream);
	void sendDrainedToUpstream() const;
	void sendToUpstream(Buffer<> *buffer) const;
	void sendToUpstream(const char *data, Size length) const;
	void sendToUpstream(const char *cString) const;
	void sendToUpstream(const String *string) const;
	Upstream *upstream() const;
};

}

/*** Inline implementations ***/

#include <unistd.h>
#include <fcntl.h>

#include "upstream.h"

namespace Stateplex {

/*
 * Constructor.
 */

inline Downstream::Downstream(Actor *actor)
	: Object(actor), mUpstream(0)
{ }

/*
 * Destructor.
 */

inline Downstream::~Downstream()
{ }

/*
 * Sets Upstream for specific upstream.
 */

inline void Downstream::setUpstream(Upstream *upstream)
{
	if (mUpstream == upstream)
		return;

	Upstream *oldUpstream = mUpstream;
	mUpstream = upstream;

	if (oldUpstream)
		oldUpstream->setDownstream(0);
	if (mUpstream)
		mUpstream->setDownstream(this);

}

/*
 * Sends pointer buffer to Upstream.
 */

inline void Downstream::sendToUpstream(Buffer<> *buffer) const
{
	mUpstream->receiveFromDownstream(buffer);
}

/*
 * Sends character data of size length to Upstream.
 */

inline void Downstream::sendToUpstream(const char *data, Size length) const
{
	mUpstream->receiveFromDownstream(data, length);
}

/*
 * Sends whole character cString to Upstream.
 */

inline void Downstream::sendToUpstream(const char *cString) const
{
	mUpstream->receiveFromDownstream(cString, strlen(cString));
}

/*
 * Sends pointer string to Upstream.
 */

inline void Downstream::sendToUpstream(const String *string) const
{
	mUpstream->receiveFromDownstream(string->chars(), string->length());
}

/*
 * Sends drained to Upstream.
 */

inline void Downstream::sendDrainedToUpstream() const
{
	mUpstream->receiveDrainedFromDownstream();
}

}

#endif
