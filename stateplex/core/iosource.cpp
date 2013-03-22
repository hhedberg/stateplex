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
 * Sets ready to read and write based on given parameters and
 * invokes handler function.
 *
 * @param readyToRead	true if ready for reading, otherwise false.
 * @param readyToWrite 	true if ready for writing, otherwise false.
 */

void IoSource::handleReady(bool readyToRead, bool readyToWrite)
{
	if (readyToRead)
		mReadyToRead = 1;
	if (readyToWrite)
		mReadyToWrite = 1;

	invokeHandler();
}

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
	mReadyToRead = 0;

	return size;
}

/** 
 * Writes data to file descriptor and returns the amount of bytes written.
 *
 * @param *data		pointer to array from which data is written to file.
 * @param length	describes the length to be written to file.
 * @return		the size in bytes that was written to file.
 */
 
Size IoSource::write(char *data, Size length)
{
	if (!mReadyToWrite)
		return 0;

	Size size = ::write(fd(), data, length);
	if (size == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			size = 0;
		} else {
			perror("read");
			abort();
		}
	}
	mReadyToWrite = 0;

	return size;
}

}
