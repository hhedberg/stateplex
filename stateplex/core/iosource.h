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
#include "../core/buffer.h"

namespace Stateplex {

/** 
 * @brief Inherited from class Source. Used by the net module classes.
 */
 
class String;

class IoSource : public Source {
	Buffer<> *mOutputBuffer;

	int mReadyToRead : 1;
	int mReachedEof : 1;

protected:
	IoSource(Actor *actor, int fd = -1, bool readable = true, bool writable = true, bool enabled = true);

	void setFd(int fd);

public:
	virtual ~IoSource();

	bool isReadable() const;
	bool isWritable() const;
	bool isReadyToRead() const;
	bool isReadyToWrite() const;
	bool hasReachedEof() const;

	Size read(char *data, Size length);
	Size read(Buffer<> *buffer);
	void write(const char *data, Size length);
	void write(const Buffer<> *buffer);
	void write(const String *string);
};

}

/*** Inline implementations ***/

namespace Stateplex {
	
/** 
 * Constructor for IoSource class.
 *
 * @param *actor		pointer to actor that is using IoSource.
 * @param fd			file descriptor.
 * @param *handlerObject	pointer to handler object.
 * @param *handlerFunction 	pointer to handler function.
 */

inline IoSource::IoSource(Actor *actor, int fd, bool readable, bool writable, bool enabled)
	: Source(actor, fd, readable, writable, enabled, false), mOutputBuffer(0), mReadyToRead(1), mReachedEof(0)
{ }

/** 
 * Destructor for IoSource class.
 */
 
inline IoSource::~IoSource()
{ }

inline bool IoSource::isReadable() const
{
	return Source::isReadable();
}

/** 
 * Checks if source writable.
 *
 * @return		true if writable, otherwise false.
 */
 
inline bool IoSource::isWritable() const
{
	return Source::isWritable();
}

/** 
 * Checks if source ready for reading.
 *
 * @return		true if ready, otherwise false.
 */
 
inline bool IoSource::isReadyToRead() const
{
	return Source::isReadable() && !!mReadyToRead;
}

/** 
 * Checks if source ready for writing.
 *
 * @return		true if ready, otherwise false.
 */

inline bool IoSource::isReadyToWrite() const
{
	return Source::isWritable() && !mOutputBuffer;
}

/** 
 * Checks if end-of-file has been reached.
 *
 * @return		true if eof has been reached, otherwise false.
 */

inline bool IoSource::hasReachedEof() const
{
	return !!mReachedEof;
}

}

#endif
