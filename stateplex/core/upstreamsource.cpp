/*
 * Stateplex - A server-side actor model library.
 *
 * core/upstreamsource.cpp
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

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "upstreamsource.h"
#include "writebuffer.h"

namespace Stateplex {

/*
 * Handles ready, read and write to buffer. 
 */

void UpstreamSource::handleReady(bool readyToRead, bool readyToWrite)
{
	if (readyToRead) {
		WriteBuffer<> buffer(actor());
		buffer.ensurePushLength(buffer.blockSize());
		ssize_t size = ::read(fd(), buffer.pushPointer(), buffer.pushLength());
		if (size == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				perror("read");
				abort();
			}
		} else if (size == 0) {
			mReadEof = true;
			sendDrainedToDownstream();
		} else {
			buffer.pushed(size);
			sendToDownstream(&buffer);
		}
	}

	if (readyToWrite && mWriteBuffer) {
	}
}

/*
 * Receives drained from Downstream.
 */

void UpstreamSource::receiveDrainedFromDownstream()
{
	mWriteEof = true;
}

/*
 * Receives character data of size length and writes to buffer.
 */

void UpstreamSource::receiveFromDownstream(const char *data, Size length)
{
	if (mWriteBuffer) {
		mWriteBuffer->append(data, length);
		return;
	}

	while (1) {
		ssize_t size = ::write(fd(), data, length);
		if (size == (ssize_t)length)
			return;

		if (size == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				perror("read");
				abort();
			}
		}
		data += size;
		length -= size;
	};

	mWriteBuffer = new WriteBuffer<>(actor());
	mWriteBuffer->append(data, length);
}

/*
 * Receives pointer buffer and writes to buffer.
 */

void UpstreamSource::receiveFromDownstream(Buffer<> *buffer)
{
	if (mWriteBuffer) {
		mWriteBuffer->append(buffer);
		return;
	}

	Buffer<>::Iterator iterator(buffer);
	Size length;
	while ((length = iterator.charBlockLength()) > 0) {
		receiveFromDownstream(iterator.charBlock(), length);
		iterator.advance(length);
	}
}

}
