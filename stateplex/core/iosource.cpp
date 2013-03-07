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

#include "iosource.h"

namespace Stateplex {

void IoSource::handleReady(bool readyToRead, bool readyToWrite)
{
	if (readyToRead)
		mReadyToRead = 1;
	if (readyToWrite)
		mReadyToWrite = 1;

	invokeHandler();
}

Size IoSource::read(char *data, Size length)
{
	if (!mReadyToRead)
		return 0;

	Size size = ::read(fd(), data, length);
	if (size == 0)
		mReachedEof = 1;
	mReadyToRead = 0;

	return size;
}

Size IoSource::write(char *data, Size length)
{
	if (!mReadyToWrite)
		return 0;

	Size size = ::write(fd(), data, length);
	mReadyToWrite = 0;

	return size;
}

}
