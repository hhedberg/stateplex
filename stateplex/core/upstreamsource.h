/*
 * Stateplex - A server-side actor model library.
 *
 * core/upstreamsource.h
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

#ifndef INCLUDED_STATEPLEX_UPSTREAM_SOURCE_H
#define INCLUDED_STATEPLEX_UPSTREAM_SOURCE_H

#include "upstream.h"
#include "source.h"
#include "writebuffer.h"

namespace Stateplex {

class UpstreamSource : public Upstream, public Source {
	WriteBuffer<> *mWriteBuffer;

	int mReadEof : 1;
	int mWriteEof : 1;

protected:
	UpstreamSource(Actor *actor, int fd = -1, bool readable = true, bool writable = true, bool enabled = true);

	virtual void handleReady(bool readyToRead, bool readyToWrite);
	virtual void receiveDrainedFromDownstream();
	virtual void receiveFromDownstream(const char *data, Size length);
	virtual void receiveFromDownstream(Buffer<> *buffer);

public:
	virtual ~UpstreamSource();
};

}

/*** Inline implementations ***/

#include <unistd.h>
#include <fcntl.h>

#include "actor.h"

namespace Stateplex {
	
inline UpstreamSource::UpstreamSource(Actor *actor, int fd, bool readable, bool writable, bool enabled)
	: Object(actor), Upstream(actor), Source(actor, fd, readable, writable, enabled), mWriteBuffer(0), mReadEof(false), mWriteEof(false)
{ }

inline UpstreamSource::~UpstreamSource()
{ }

}

#endif
