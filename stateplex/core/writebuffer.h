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

template<Size16 mBlockSize>
class GenericWriteBuffer : public GenericReadBuffer<mBlockSize> {
	typename GenericBuffer<mBlockSize>::Block *allocateBlock(typename GenericBuffer<mBlockSize>::Block *previousBlock);
	typename GenericBuffer<mBlockSize>::Block *ensurePush(typename GenericBuffer<mBlockSize>::Block *block, Size length);
	void pushToBlock(const char *cString, Size length, typename GenericBuffer<mBlockSize>::Block *block);
	typename GenericBuffer<mBlockSize>::Block *splitBlock(Size offset);

public:
	GenericWriteBuffer(Actor *actor);

	void append(GenericBuffer<mBlockSize> *buffer);
	void append(const char *cString);
	void append(const char *cString, Size length);
	void append(const String *string);

	void ensurePushLength(Size16 length);
	char *pushPointer() const;
	Size16 pushLength() const;
	void pushed(Size16 length);

	void insert(Size offset, GenericBuffer<mBlockSize> *buffer);
	void insert(Size insertOffset, GenericBuffer<mBlockSize> *buffer, Size offset, Size length);
	void insert(Size offset, const char *cString);
	void insert(Size offset, const char *cString, Size length);
};

typedef GenericWriteBuffer<1024> WriteBuffer;

}

/*** Template implementations ***/

#include "../core/actor.h"

namespace Stateplex {


template<Size16 mBlockSize>
typename GenericBuffer<mBlockSize>::Block *GenericWriteBuffer<mBlockSize>::allocateBlock(typename GenericBuffer<mBlockSize>::Block *previousBlock)
{
	void *memory = GenericBuffer<mBlockSize>::Block::allocateMemory(this->allocator());
	typename GenericBuffer<mBlockSize>::Block *block = new(memory) typename GenericBuffer<mBlockSize>::Block(this->allocator());
	if (previousBlock)
		block->addAfter(previousBlock);
	else
		this->mBlocks.addHead(block);

	return block;
}

template<Size16 mBlockSize>
typename GenericBuffer<mBlockSize>::Block *GenericWriteBuffer<mBlockSize>::ensurePush(typename GenericBuffer<mBlockSize>::Block *block, Size length)
{
	if (!block || length < block->room())
		block = this->allocateBlock(block);

	return block;
}

template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::pushToBlock(const char *cString, Size length, typename GenericBuffer<mBlockSize>::Block *block)
{
	block = ensurePush(block, 1);
	Size16 room = block->room();
	if (length < room) {
		block->copyFrom(cString, length);
		this->mSize += length;
	} else {
		block->copyFrom(cString, room);
		this->mSize += room;

		pushToBlock(cString + room, length - room, block);
	}
}


template<Size16 mBlockSize>
typename GenericBuffer<mBlockSize>::Block *GenericWriteBuffer<mBlockSize>::splitBlock(Size offset)
{
	if (offset == 0)
		return 0;

	for (typename GenericBuffer<mBlockSize>::Block *block = this->mBlocks.first(); block; block = this->mBlocks.next(block)) {
		Size16 size = block->size();
		if (offset <= size) {
			if (offset > 0 && offset < size)
				block->split(this->allocator(), offset);
			return block;
		}
		offset -= size;
	}

	return 0;
}

template<Size16 mBlockSize>
GenericWriteBuffer<mBlockSize>::GenericWriteBuffer(Actor *actor)
	: GenericReadBuffer<mBlockSize>(actor)
{ }

/**
 * Copies the contents of the given buffer into the end of this buffer.
 * The operation is fast, because only references to the actual bytes are copied internally.
 * 
 * @param buffer	target buffer to be added.
 */

template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::append(GenericBuffer<mBlockSize> *buffer)
{
	typename GenericBuffer<mBlockSize>::Block *previousBlock = this->mBlocks.last();
	for (ListIterator<typename GenericBuffer<mBlockSize>::Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = GenericBuffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename GenericBuffer<mBlockSize>::Block *block = new(memory) typename GenericBuffer<mBlockSize>::Block(this->allocator(), iterator.current());
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			this->mBlocks.addTail(block);
		this->mSize += block->size();
		previousBlock = block;
	}
}

/**
 * Copies the C string into the end of the buffer.
 * 
 * @param string	string to be added
 */
template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::append(const char *cString)
{
	append(cString, strlen(cString));
}

/**
 * Copies the given amount of bytes from the specified location into the end of the buffer.
 */
template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::append(const char *cString, Size length)
{
	this->pushToBlock(cString, length, this->mBlocks.last());
}

/**
 * Copies the contents of the given String into the end of the buffer.
 */
template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::append(const String *string)
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
void GenericWriteBuffer<mBlockSize>::ensurePushLength(Size16 length)
{
	this->ensurePush(this->mBlocks.last(), length);
}

/**
 * Function that returns pointer to next of the last block saved.
 * 
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *GenericWriteBuffer<mBlockSize>::pushPointer() const
{
	typename GenericBuffer<mBlockSize>::Block *block = this->mBlocks.last();
	return block->endPointer();
}

/**
 * Function that returns last pushed blocks length.
 * 
 * @return	lenght of the push.
 */

template<Size16 mBlockSize>
Size16 GenericWriteBuffer<mBlockSize>::pushLength() const
{
	typename GenericBuffer<mBlockSize>::Block *block = this->mBlocks.last();
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
void GenericWriteBuffer<mBlockSize>::pushed(Size16 length)
{
	typename GenericBuffer<mBlockSize>::Block *block = this->mBlocks.last();
	block->pushed(length);
	this->mSize += length;
}

template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::insert(Size offset, GenericBuffer<mBlockSize> *buffer)
{
	typename GenericBuffer<mBlockSize>::Block *previousBlock = this->splitBlock(offset);
	for (ListIterator<typename GenericBuffer<mBlockSize>::Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = typename GenericBuffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename GenericBuffer<mBlockSize>::Block *block = new(memory) typename GenericBuffer<mBlockSize>::Block(iterator.current());
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			this->mBlocks.addHead(block);
		this->mSize += block->size();
		previousBlock = block;
	}
}

template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::insert(Size insertOffset, GenericBuffer<mBlockSize> *buffer, Size offset, Size length)
{
	typename GenericBuffer<mBlockSize>::Block *previousBlock = this->splitBlock(insertOffset);
	for (typename GenericBuffer<mBlockSize>::Block *block = this->blockByOffset(&offset); block && length; block = this->mBlocks.next(block)) {
		Size size;
		if (offset + length < block->size())
			size = length;
		else
			size = block->size();
		void *memory = typename GenericBuffer<mBlockSize>::Block::allocateMemory(this->allocator());
		typename GenericBuffer<mBlockSize>::Block *newBlock = new(memory) typename GenericBuffer<mBlockSize>::Block(this->allocator(), block, offset, size);
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
void GenericWriteBuffer<mBlockSize>::insert(Size offset, const char *cString)
{
	insert(offset, cString, strlen(cString));
}

template<Size16 mBlockSize>
void GenericWriteBuffer<mBlockSize>::insert(Size offset, const char *cString, Size length)
{
	typename GenericBuffer<mBlockSize>::Block *previousBlock = this->splitBlock(offset);
	this->pushToBlock(cString, strlen(cString), previousBlock);
}

}

#endif
