/*
 * Stateplex - A server-side actor model library.
 *
 * core/source.h
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

#ifndef INCLUDED_STATEPLEX_IO_SOURCE_H
#define INCLUDED_STATEPLEX_IO_SOURCE_H

#include "source.h"
#include "../core/types.h"

namespace Stateplex {

class IoSource : public Source {
	Method mHandler;

	int mReadable : 1;
	int mWritable : 1;
	int mReadyToRead : 1;
	int mReadyToWrite : 1;
	int mReachedEof : 1;

protected:
	template<typename T> IoSource(Actor *actor, int fd, T *handlerObject, void (T::*handlerFunction)(Source *source));

	void invokeHandler();
	void handleReady(bool readyToRead, bool readyToWrite);
	void setMode(bool readable, bool writable);

public:
	virtual ~IoSource();

	bool isReadable() const;
	bool isWritable() const;
	bool isReadyToRead() const;
	bool isReadyToWrite() const;
	bool hasReachedEof() const;

	Size read(char *data, Size length);
	Size write(char *data, Size length);
	template<typename T> void setHandler(T *handlerObject, void (T::*handlerFunction)(IoSource *source));
};

}

/*** Inline implementations ***/

namespace Stateplex {

template<typename T>
IoSource::IoSource(Actor *actor, int fd, T *handlerObject, void (T::*handlerFunction)(Source *source))
	: Source(actor, fd, handlerObject, handlerFunction), mReadable(0), mWritable(0), mReadyToRead(0), mReadyToWrite(1), mReachedEof(0)
{ }

inline IoSource::~IoSource()
{ }

inline void IoSource::invokeHandler()
{
	mHandler.invoke(this);
}

inline void IoSource::setMode(bool readable, bool writable)
{
	mReadable = readable ? 1 : 0;
	mWritable = writable ? 1 : 0;
}

inline bool IoSource::isReadable() const
{
	return !!mReadable;
}
inline bool IoSource::isWritable() const
{
	return !!mWritable;
}

inline bool IoSource::isReadyToRead() const
{
	return !!mReadable && !!mReadyToRead;
}

inline bool IoSource::isReadyToWrite() const
{
	return !!mWritable && !!mReadyToWrite;
}

inline bool IoSource::hasReachedEof() const
{
	return !!mReachedEof;
}

template<typename T>
void IoSource::setHandler(T *handlerObject, void (T::*handlerFunction)(IoSource *source))
{
	mHandler.set(handlerObject, handlerFunction);
}

}

#endif
