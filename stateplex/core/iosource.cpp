/*
 * Stateplex - A server-side actor model library.
 *
 * core/opsource.h
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

#include "iosource.h"

namespace Stateplex {
	
/** 
 * Reads data from file descriptor and returns the amount of bytes read.
 *
 * @param *data		pointer to array to which the data is written.
 * @param length	describes the length to be read.
 * @return		the size in bytes that was read from file.
 */
 
Size IoSource::read(char *data, Size length)
{
	if (!mReadyToRead)
		return 0;

	Size size = ::read(fd(), data, length);
	if (size == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			size = 0;
		} else {
			perror("read");
			abort();
		}
	} else if (size == 0) {
		mReachedEof = 1;
	}
	mReadyToRead = 1;

	return size;
}

Size IoSource::read(WriteBuffer<> *buffer)
{
	buffer->ensurePushLength(buffer->blockSize() / 4);
	Size size = read(buffer->pushPointer(), buffer->pushLength());
	buffer->pushed(size);
	return size;
}

/** 
 * Writes data to file descriptor and returns the amount of bytes written.
 *
 * @param *data		pointer to array from which data is written to file.
 * @param length	describes the length to be written to file.
 * @return		the size in bytes that was written to file.
 */
 
void IoSource::write(const char *data, Size length)
{
	if (mOutputBuffer) {
		/* TODO: Buffer */
		return;
	}

	Size size = ::write(fd(), data, length);
	if (size == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			size = 0;
		} else {
			perror("read");
			abort();
		}
	} else if (size != length) {
		/* TODO: Buffer */
	}
}

void IoSource::write(const Buffer<> *buffer)
{
	Buffer<>::Iterator iterator(buffer);
	Size length;
	while ((length = iterator.charBlockLength()) > 0) {
		write(iterator.charBlock(), length);
		iterator.advance(length);
	}
}

void IoSource::write(const String *string)
{
	write(string->chars(), string->length());
}

}
