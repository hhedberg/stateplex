/*
 * Stateplex - A server-side actor model library.
 *
 * core/echostream.cpp
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

#include "echostream.h"

namespace Stateplex {

/*
 * Receives drained from Upstream.
 */

void EchoStream::receiveDrainedFromUpstream()
{
	sendDrainedToUpstream();
}

/*
 * Receives character pointer data of size length from Upstream.
 */

void EchoStream::receiveFromUpstream(const char *data, Size length)
{
	sendToUpstream(data, length);
}

/*
 * Receives pointer buffer from Upstream.
 */

void EchoStream::receiveFromUpstream(Buffer<> *buffer)
{
	sendToUpstream(buffer);
}

}
