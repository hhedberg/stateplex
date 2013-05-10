/*
 * Stateplex - A server-side actor model library.
 *
 * core/receiversource.h
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

#ifndef INCLUDED_STATEPLEX_RECEIVER_SOURCE_H
#define INCLUDED_STATEPLEX_RECEIVER_SOURCE_H

#include "source.h"
#include "receiver.h"
#include "writebuffer.h"

namespace Stateplex {

class ReceiverSource : public Source, public Receiver {
	WriteBuffer *mWriteBuffer;
	Receiver *mReceiver;

	int mReadEof : 1;
	int mWriteEof : 1;

	bool receive(const char *data, Size length);

protected:
	ReceiverSource(Actor *actor, int fd = -1, bool readable = true, bool writable = true, bool enabled = true);

	virtual void handleReady(bool readyToRead, bool readyToWrite);
	virtual void receiveEnd();
	virtual bool receive(const String *string);
	virtual bool receive(Buffer *buffer);

public:
	virtual ~ReceiverSource();

	void setReceiver(Receiver *receiver);
};

}

/*** Inline implementations ***/

#include <unistd.h>
#include <fcntl.h>

#include "actor.h"

namespace Stateplex {

/*
 * Constructor.
 */
	
inline ReceiverSource::ReceiverSource(Actor *actor, int fd, bool readable, bool writable, bool enabled)
	: Source(actor, fd, readable, writable, enabled), mWriteBuffer(0), mReceiver(0), mReadEof(false), mWriteEof(false)
{ }

/* 
 * Destructor.
 */

inline ReceiverSource::~ReceiverSource()
{ }

inline void ReceiverSource::setReceiver(Receiver *receiver)
{
	mReceiver = receiver;
}

}

#endif
