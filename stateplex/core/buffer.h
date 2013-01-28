/*
 * Stateplex - A server-side actor model library.
 *
 * core/buffer.h
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

#ifndef INCLUDED_STATEPLEX_BUFFER_H
#define INCLUDED_STATEPLEX_BUFFER_H

#include "list.h"
#include "types.h"
#include "allocator.h"

namespace Stateplex {

template<Size16 blockSize = 4096>
class Buffer {
	struct Block : public ListItem<Block> {
		Size16 mStart;
		Size16 mEnd;
	};

	List<Block> mBlocks;
	Size mSize;

	Block *mHere;
	Size16 mPosition;
	Size mOffset;

	Block *allocateBlock();
	void deallocateBlock(Block *block);
	void pushToBlock(const char *cString, Size length, Block *block);

public:
	Buffer();
	Buffer(const char *string);

	void push(Buffer *buffer);
	void push(const char *cString);
	void push(const char *cString, Size length);

	void ensurePushLength(Size16 length);
	char *pushPointer();
	Size16 pushLength();
	void pushed(Size16 length);

	char peek();
	char pop();

	char *popPointer();
	Size16 popLength();
	void popped(Size16 length);

	Size size();

	char here();
	void next();
	Size left();
	void popHere();

	void compress();
};

}

/*** Template implementations ***/

#include <string.h>

namespace Stateplex {

template<Size16 blockSize>
typename Buffer<blockSize>::Block *Buffer<blockSize>::allocateBlock()
{
	Block *block;
	/* TODO: Allocate */
	mBlocks.addTail(block);

	if (!mHere)
		mHere = block;

	return block;
}

template<Size16 blockSize>
void Buffer<blockSize>::deallocateBlock(Block *block)
{
	block->remove();
	/* TODO: deallocate */
}

template<Size16 blockSize>
void Buffer<blockSize>::pushToBlock(const char *cString, Size length, Block *block)
{
	if (!block ||block->mEnd == blockSize)
		block = allocateBlock();

	char *pointer = reinterpret_cast<char *>(block) + sizeof(Block);
	Size16 room = blockSize - block->mEnd;
	if (length < room) {
		memcpy(pointer + block->mEnd, cString, length);
		block->mEnd += length;
	} else {
		memcpy(pointer + block->mEnd, cString, room);
		pushToBlock(cString + room, length - room, 0);
	}
}

template<Size16 blockSize>
Buffer<blockSize>::Buffer()
	: mSize(0), mHere(0), mPosition(0), mOffset(0)
{ }

template<Size16 blockSize>
Buffer<blockSize>::Buffer(const char *string)
	: mSize(0), mHere(0), mPosition(0), mOffset(0)
{
	push(string);
}

template<Size16 blockSize>
void Buffer<blockSize>::push(Buffer *buffer)
{
	for (ListItem<Block> *item = buffer->mBlocks; item; item = buffer->mBlocks.next(item)) {
		Block *block = static_cast<Block *>(item);
		mBlocks.addTail(item);
		mSize += block->mEnd - block->mStart;
	}
	buffer->mSize = 0;
}

template<Size16 blockSize>
void Buffer<blockSize>::push(const char *string)
{
	push(string, strlen(string));
}

template<Size16 blockSize>
void Buffer<blockSize>::push(const char *cString, Size length)
{
	pushToBlock(cString, length, static_cast<Block *>(mBlocks.last()));
	mSize += length;
}

template<Size16 blockSize>
void Buffer<blockSize>::ensurePushLength(Size16 length)
{
	Block *block = mBlocks.last();
	if (!block || blockSize - block->mEnd < length)
		allocateBlock();
}

template<Size16 blockSize>
char *Buffer<blockSize>::pushPointer()
{
	Block *block = mBlocks.last();
	return reinterpret_cast<char *>(block) + sizeof(Block) + block->mEnd;
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::pushLength()
{
	Block *block = mBlocks.last();
	return blockSize - block->mEnd;
}

template<Size16 blockSize>
void Buffer<blockSize>::pushed(Size16 length)
{
	Block *block = mBlocks.last();
	block->mEnd += length;
	mSize += length;
}

template<Size16 blockSize>
char Buffer<blockSize>::peek()
{
	return *popPointer();
}

template<Size16 blockSize>
char Buffer<blockSize>::pop()
{
	char c = *popPointer();
	popped(1);
	return c;
}

template<Size16 blockSize>
char *Buffer<blockSize>::popPointer()
{
	Block *block = mBlocks.first();
	return reinterpret_cast<char *>(block) + sizeof(Block) + block->mStart;
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::popLength()
{
	Block *block = mBlocks.first();
	return block->mEnd - block->mStart;
}

template<Size16 blockSize>
void Buffer<blockSize>::popped(Size16 length)
{
	ListItem<Block> *item = mBlocks.first();
	Block *block = static_cast<Block *>(item);
	block->mStart += length;
	if (block->mStart >= block->mEnd) {
		if (mBlocks.next(item))
			deallocateBlock(block);
		else
			block->mStart = block->mEnd = 0;
	}
	mSize -= length;
	/* TODO: Here */
}

template<Size16 blockSize>
Size Buffer<blockSize>::size()
{
	return mSize;
}

template<Size16 blockSize>
char Buffer<blockSize>::here()
{
	return *reinterpret_cast<char *>(mHere) + sizeof(Block) + mPosition;
}

template<Size16 blockSize>
void Buffer<blockSize>::next()
{
	mPosition++;
	if (mPosition >= mHere->mEnd) {
		mHere = static_cast<Block *>(mBlocks.next(mHere));
		mPosition = mHere->mStart;
	}
	mOffset++;
}

template<Size16 blockSize>
Size Buffer<blockSize>::left()
{
	return mSize - mOffset;
}

template<Size16 blockSize>
void Buffer<blockSize>::popHere()
{
	for (ListItem<Block> *item = mBlocks.previous(mHere); item; item = mBlocks.previous(item)) {
		Block *block = static_cast<Block *>(item);
		mSize -= block->mEnd - block->mStart;
		deallocateBlock(block);
	}
	popped(mPosition - mHere->mStart);

	mHere = static_cast<Block *>(mBlocks.first());
	mPosition = mHere->mStart;
	mOffset = 0;
}

template<Size16 blockSize>
void Buffer<blockSize>::compress()
{
	/* TODO: Not implemented */
}

}

#endif
