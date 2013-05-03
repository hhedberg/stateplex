/*
 * Stateplex - A server-side actor model library.
 *
 * core/echostream.h
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

#ifndef INCLUDED_STATEPLEX_ECHO_STREAM_H
#define INCLUDED_STATEPLEX_ECHO_STREAM_H

#include "downstream.h"

namespace Stateplex {

class EchoStream : public Downstream {
protected:
	virtual void receiveDrainedFromUpstream();
	virtual void receiveFromUpstream(const char *data, Size length);
	virtual void receiveFromUpstream(Buffer<> *buffer);

public:
	EchoStream(Actor *actor);
	virtual ~EchoStream();

};

}

/*** Inline implementations ***/

#include "upstream.h"

namespace Stateplex {

/*
 * Constructor.
 */

inline EchoStream::EchoStream(Actor *actor)
	: Object(actor), Downstream(actor)
{ }

/*
 * Destructor.
 */

inline EchoStream::~EchoStream()
{ }

}

#endif
