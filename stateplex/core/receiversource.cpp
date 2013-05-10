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

#include "receiversource.h"
#include "writebuffer.h"

namespace Stateplex {

/*
 * Handles ready, read and write to buffer. 
 */

void ReceiverSource::handleReady(bool readyToRead, bool readyToWrite)
{
	if (readyToRead) {
		WriteBuffer buffer(actor());
		buffer.ensurePushLength(buffer.blockSize());
		ssize_t size = ::read(fd(), buffer.pushPointer(), buffer.pushLength());
		if (size == -1) {
			if (errno == ECONNRESET) {
				mReadEof = true;
				mWriteEof = true;
				// TODO: Clear mWriteBuffer
				mReceiver->receiveEnd();
		} else if (errno != EAGAIN && errno != EWOULDBLOCK) {
				perror("read");
				abort();
			}
		} else if (size == 0) {
			mReadEof = true;
			mReceiver->receiveEnd();
		} else {
			buffer.pushed(size);
			mReceiver->receive(&buffer);
			// TODO: If receive() returns false, do not read any more
		}
	}

	if (readyToWrite && mWriteBuffer) {
	}
}

/*
 * Sets that the receiving has come to and end.
 */

void ReceiverSource::receiveEnd()
{
	mWriteEof = true;
}

/*
 * Receives character data of size length and writes to buffer.
 */

bool ReceiverSource::receive(const char *data, Size length)
{
	if (mWriteEof)
		return false;

	if (mWriteBuffer) {
		mWriteBuffer->append(data, length);
		return true;
	}

	while (1) {
		ssize_t size = ::write(fd(), data, length);
		if (size == (ssize_t)length)
			return true;

		if (size == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else if (errno == ECONNRESET) {
				mWriteEof = true;
				return false;
			} else {
				perror("read");
				abort();
			}
		}
		data += size;
		length -= size;
	};

	mWriteBuffer = new WriteBuffer(actor());
	mWriteBuffer->append(data, length);

	return true;
}

bool ReceiverSource::receive(const String *string)
{
	return receive(string->chars(), string->length());
}

/*
 * Receives data and writes them to target Buffer
 * 
 * @param *buffer	target Buffer to write.
 */

bool ReceiverSource::receive(Buffer *buffer)
{
	if (mWriteBuffer) {
		mWriteBuffer->append(buffer);
		return true;
	}

	Buffer::Iterator iterator(buffer);
	Size length;
	while ((length = iterator.charBlockLength()) > 0) {
		if (!receive(iterator.charBlock(), length))
			return false;
		iterator.advance(length);
	}

	return true;
}

}
