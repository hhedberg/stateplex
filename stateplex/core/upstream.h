/*
 * Stateplex - A server-side actor model library.
 *
 * core/upstream.h
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

#ifndef INCLUDED_STATEPLEX_UPSTREAM_H
#define INCLUDED_STATEPLEX_UPSTREAM_H

#include "object.h"
#include "types.h"
#include "buffer.h"

namespace Stateplex {

class Downstream;
class String;

class Upstream : public virtual Object {
	friend class Downstream;

	Downstream *mDownstream;

protected:
	Upstream(Actor *actor);

	virtual void receiveDrainedFromDownstream() = 0;
	virtual void receiveFromDownstream(const char *data, Size length) = 0;
	virtual void receiveFromDownstream(Buffer<> *buffer) = 0;

public:
	virtual ~Upstream();

	Downstream *downstream() const;
	void setDownstream(Downstream *downstream);
	void sendDrainedToDownstream() const;
	void sendToDownstream(Buffer<> *buffer) const;
	void sendToDownstream(const char *data, Size length) const;
	void sendToDownstream(const char *cString) const;
	void sendToDownstream(const String *string) const;
};

}

/*** Inline implementations ***/

#include "downstream.h"
#include "string.h"

namespace Stateplex {

/*
 * Constructor.
 */

inline Upstream::Upstream(Actor *actor)
	: Object(actor), mDownstream(0)
{ }

/*
 * Destructor
 */

inline Upstream::~Upstream()
{ }

/*
 * Sets Downstream for specific downstream.
 */

inline void Upstream::setDownstream(Downstream *downstream)
{
	if (mDownstream == downstream)
		return;

	Downstream *oldDownstream = mDownstream;
	mDownstream = downstream;

	if (oldDownstream)
		oldDownstream->setUpstream(0);
	if (mDownstream)
		mDownstream->setUpstream(this);
}

/*
 * Sends buffer to Downstream.
 */

inline void Upstream::sendToDownstream(Buffer<> *buffer) const
{
	mDownstream->receiveFromUpstream(buffer);
}

/*
 * Sends character pointer data of size length to Downstream.
 */

inline void Upstream::sendToDownstream(const char *data, Size length) const
{
	mDownstream->receiveFromUpstream(data, length);
}

/*
 * Sends whole string to Downstream.
 */

inline void Upstream::sendToDownstream(const char *cString) const
{
	mDownstream->receiveFromUpstream(cString, strlen(cString));
}

/*
 * Sends string length to Downstream.
 */

inline void Upstream::sendToDownstream(const String *string) const
{
	mDownstream->receiveFromUpstream(string->chars(), string->length());
}

/*
 * Sends drained to Downstream.
 */

inline void Upstream::sendDrainedToDownstream() const
{
	mDownstream->receiveDrainedFromUpstream();
}

}

#endif
