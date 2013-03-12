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

/** 
 * @brief Class IoSource.
 *
 * Class IoSource inherited from class Source. Used for...
 */
 
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
	
/** 
 * Constructor for IoSource class.
 *
 * @param *actor			desc
 * @param fd				desc
 * @param *handlerObject	desc
 * @param *handlerFunction 	desc
 */

template<typename T>
IoSource::IoSource(Actor *actor, int fd, T *handlerObject, void (T::*handlerFunction)(Source *source))
	: Source(actor, fd, handlerObject, handlerFunction), mReadable(0), mWritable(0), mReadyToRead(0), mReadyToWrite(1), mReachedEof(0)
{ }

/** 
 * Destructor for IoSource class.
 *
 */
 
inline IoSource::~IoSource()
{ }

/** 
 * Invoke
 *
 */
 
inline void IoSource::invokeHandler()
{
	mHandler.invoke(this);
}

/** 
 * Sets mode based on parameter values.
 *
 * @param readable	true if readable, otherwise false.
 * @param writable	true if writable, otherwise false.
 */
 
inline void IoSource::setMode(bool readable, bool writable)
{
	mReadable = readable ? 1 : 0;
	mWritable = writable ? 1 : 0;
}

/** 
 * Checks if readable.
 *
 * @return	true if readable, otherwise false.
 */
 
inline bool IoSource::isReadable() const
{
	return !!mReadable;
}

/** 
 * Checks if writable.
 *
 * @return	true if writable, otherwise false.
 */
 
inline bool IoSource::isWritable() const
{
	return !!mWritable;
}

/** 
 * Checks if ready for reading.
 *
 * @return	true if ready, otherwise false.
 */
 
inline bool IoSource::isReadyToRead() const
{
	return !!mReadable && !!mReadyToRead;
}

/** 
 * Checks if ready for writing.
 *
 * @return	true if ready, otherwise false.
 */

inline bool IoSource::isReadyToWrite() const
{
	return !!mWritable && !!mReadyToWrite;
}

/** 
 * Checks if end-of-file has been reached.
 *
 * @return	true if eof has been reached, otherwise false.
 */

inline bool IoSource::hasReachedEof() const
{
	return !!mReachedEof;
}

/** 
 * Sets handler.
 * 
 * @param *handlerObject	desc
 * @param *handlerFunction	function that takes a pointer to IoSource type.
 */
 
template<typename T>
void IoSource::setHandler(T *handlerObject, void (T::*handlerFunction)(IoSource *source))
{
	mHandler.set(handlerObject, handlerFunction);
}

}

#endif
