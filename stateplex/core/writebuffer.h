/*
 * Stateplex - A server-side actor model library.
 *
 * core/writebuffer.h
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

#ifndef INCLUDED_STATEPLEX_WRITE_BUFFER_H
#define INCLUDED_STATEPLEX_WRITE_BUFFER_H

#include "readbuffer.h"

namespace Stateplex {

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
class WriteBuffer : public ReadBuffer<mBlockSize> {
public:
	WriteBuffer(Actor *actor);

	void append(Buffer<> *buffer);
	void append(const char *cString);
	void append(const char *cString, Size length);
	void append(const String *string);

	void ensurePushLength(Size16 length);
	char *pushPointer() const;
	Size16 pushLength() const;
	void pushed(Size16 length);

	void insert(Size offset, Buffer<> *buffer);
	void insert(Size insertOffset, Buffer<> *buffer, Size offset, Size length);
	void insert(Size offset, const char *cString);
	void insert(Size offset, const char *cString, Size length);
};

}

/*** Template implementations ***/

#include "../core/actor.h"

namespace Stateplex {

template<Size16 mBlockSize>
WriteBuffer<mBlockSize>::WriteBuffer(Actor *actor)
	: ReadBuffer<mBlockSize>(actor)
{ }

/**
 * Copies the contents of the given buffer into the end of this buffer.
 * The operation is fast, because only references to the actual bytes are copied internally.
 * 
 * @param buffer	target buffer to be added.
 */

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::append(Buffer<> *buffer)
{
	typename Buffer<mBlockSize>::Block *previousBlock = this->mBlocks.last();
	for (ListIterator<typename Buffer<mBlockSize>::Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = Buffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename Buffer<mBlockSize>::Block *block = new(memory) typename Buffer<mBlockSize>::Block(this->allocator(), iterator.current());
		block->addAfter(previousBlock);
		this->mSize += block->size();
		previousBlock = block;
	}
}

/**
 * Copies the C string into this buffer.
 * 
 * @param string	string to be added
 */

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::append(const char *cString)
{
	append(cString, strlen(cString));
}

/**
 * Function that takes in chars and a value how many chars are to be
 * added to block. After this the function calls another function
 * to push the chars to a block.
 * 
 * @param *cString	chars to be added.
 * @param length	determines how many chars are to be added.
 */

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::append(const char *cString, Size length)
{
	this->pushToBlock(cString, length, this->mBlocks.last());
}

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::append(const String *string)
{
	this->pushToBlock(string->chars(), string->length(), this->mBlocks.last());
}


/**
 * Function that ensures push length.
 * It check that there are blocks if none allocates a new one.
 * If there are blocks it ensures that push length is not too big
 * to be allocated.
 * 
 * @param length	maximum push length.
 */

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::ensurePushLength(Size16 length)
{
	this->ensurePush(this->mBlocks.last(), length);
}

/**
 * Function that returns pointer to next of the last block saved.
 * 
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *WriteBuffer<mBlockSize>::pushPointer() const
{
	typename Buffer<mBlockSize>::Block *block = this->mBlocks.last();
	return block->endPointer();
}

/**
 * Function that returns last pushed blocks length.
 * 
 * @return	lenght of the push.
 */

template<Size16 mBlockSize>
Size16 WriteBuffer<mBlockSize>::pushLength() const
{
	typename Buffer<mBlockSize>::Block *block = this->mBlocks.last();
	return block->room();
}

/**
 * Function that is called when a new block is added to buffer.
 * This function takes a value and adds it to a variable that represends the
 * blocks last address space. Also this value is added to the buffer size.
 * 
 * @param length	value to be assigned.
 */

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::pushed(Size16 length)
{
	typename Buffer<mBlockSize>::Block *block = this->mBlocks.last();
	block->pushed(length);
	this->mSize += length;
}

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::insert(Size offset, Buffer<> *buffer)
{
	typename Buffer<mBlockSize>::Block *previousBlock = this->splitBlock(offset);
	for (ListIterator<typename Buffer<mBlockSize>::Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = typename Buffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename Buffer<mBlockSize>::Block *block = new(memory) typename Buffer<mBlockSize>::Block(iterator.current());
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			this->mBlocks.addHead(block);
		this->mSize += block->size();
		previousBlock = block;
	}
}

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::insert(Size insertOffset, Buffer<> *buffer, Size offset, Size length)
{
	typename Buffer<mBlockSize>::Block *previousBlock = this->splitBlock(insertOffset);
	for (typename Buffer<mBlockSize>::Block *block = this->blockByOffset(&offset); block && length; block = this->mBlocks.next(block)) {
		Size size;
		if (offset + length < block->size())
			size = length;
		else
			size = block->size();
		void *memory = typename Buffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename Buffer<mBlockSize>::Block *newBlock = new(memory) typename Buffer<mBlockSize>::Block(this->allocator(), block, offset, size);
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			this->mBlocks.addHead(block);
		this->mSize += size;
		length -= size;
		previousBlock = block;
	}
}

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::insert(Size offset, const char *cString)
{
	insert(offset, cString, strlen(cString));
}

template<Size16 mBlockSize>
void WriteBuffer<mBlockSize>::insert(Size offset, const char *cString, Size length)
{
	typename Buffer<mBlockSize>::Block *previousBlock = this->splitBlock(offset);
	this->pushToBlock(cString, strlen(cString), previousBlock);
}

}

#endif
