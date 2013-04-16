/*
 * Stateplex - A server-side actor model library.
 *
 * core/readbuffer.h
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

#ifndef INCLUDED_STATEPLEX_READ_BUFFER_H
#define INCLUDED_STATEPLEX_READ_BUFFER_H

#include "buffer.h"

namespace Stateplex {

template<Size16 blockSize> class WriteBuffer;

/**
 * Holds bytes.
 *
 * The bytes are internally stored in chained blocks. Operations between buffers are fast, because no actual
 * byte data is copied. Instead, just references to the data are maintained.
 *
 * The interface is divided into three classes:
 * - Buffer provides functions to query the contents of a buffer.
 * - ReadBuffer provides functions to consume data from a buffer.
 * - WriteBuffer provides functions to produce data into a buffer.
 *
 * When constructing a new buffer, the WriteBuffer class must be used.
 */

template<Size16 mBlockSize = 1024>
class ReadBuffer : public Buffer<mBlockSize> {
	friend class WriteBuffer<mBlockSize>;

	ReadBuffer(Actor *actor);

	void deleteBlock(typename Buffer<mBlockSize>::Block *block);

public:
	char peek() const;
	char pop();

	const char *popPointer() const;
	Size16 popLength() const;
	void popped(Size16 length);
	void poppedAll();
};

}

/*** Template implementations ***/

namespace Stateplex {

template<Size16 mBlockSize>
ReadBuffer<mBlockSize>::ReadBuffer(Actor *actor)
	: Buffer<mBlockSize>(actor)
{ }

template<Size16 mBlockSize>
void ReadBuffer<mBlockSize>::deleteBlock(typename Buffer<mBlockSize>::Block *block)
{
	for (typename Buffer<mBlockSize>::Iterator *iterator = this->mIterators.first(); iterator; iterator = this->mIterators.next(iterator)) {
		iterator->blockDeleted(block);
	}
	block->destroy(this->allocator());
}

/**
 * Returns the first byte in the buffer.
 */
template<Size16 mBlockSize>
char ReadBuffer<mBlockSize>::peek() const
{
	return *popPointer();
}

/**
 * Returns the first byte in the buffer and removes it from the buffer.
 * 
 * @return	value that was popped.
 */

template<Size16 mBlockSize>
char ReadBuffer<mBlockSize>::pop()
{
	char c = *popPointer();
	popped(1);
	return c;
}

/**
 * Returns a pointer to the memory area in the beginning of the buffer. This function is used
 * to access multiple bytes at once.
 * 
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
const char *ReadBuffer<mBlockSize>::popPointer() const
{
	typename Buffer<mBlockSize>::Block *block = this->mBlocks.first();
	return reinterpret_cast<char *>(block) + sizeof(typename Buffer<mBlockSize>::Block) + block->start();
}

/**
 * Returns the length of the memory area given by the popPointer() function.
 * The length is at maximum blockSize(). The area may not contain all bytes in the buffer,
 * but just the bytes that fit internally to the first block.
 * 
 * @return	the size of the first block.
 */

template<Size16 mBlockSize>
Size16 ReadBuffer<mBlockSize>::popLength() const
{
	typename Buffer<mBlockSize>::Block *block = this->mBlocks.first();
	return block->mEnd - block->mStart;
}

/**
 * Removes given amount of bytes from the beginning of the buffer.
 *
 * @param length	length of the value that has been popped.
 */

template<Size16 mBlockSize>
void ReadBuffer<mBlockSize>::popped(Size16 length)
{
	for (ListIterator<typename Buffer<mBlockSize>::Block> iterator(&this->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		typename Buffer<mBlockSize>::Block *block = iterator.current();
		Size16 size = block->size();
		if (size > length) {
			block->popped(length);
			break;
		} else {
			deleteBlock(block);
			length -= size;
		}
	}
	this->mSize -= length;
	/* TODO: Here */
}

template<Size16 mBlockSize>
void ReadBuffer<mBlockSize>::poppedAll()
{
	popped(this->mSize);
}

}

#endif
