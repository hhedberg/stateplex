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

#include "object.h"
#include "list.h"
#include "types.h"

namespace Stateplex {

class Allocator;
class String;
template<Size16 blockSize> class ReadBuffer;
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
class Buffer : public Object {
	friend class ReadBuffer<mBlockSize>;
	friend class WriteBuffer<mBlockSize>;

	class Block : public ListItem {
		struct Bytes {
			Size32 mReferenceCount;
			Size16 mStart;
			Size16 mEnd;
		};

		Bytes *mBytes;
		Size16 mStart;
		Size16 mEnd;

	public:
		Block(Allocator *allocator);
		Block(Allocator *allocator, Block *block);
		Block(Allocator *allocator, Block *block, Size16 offset, Size16 length);

		Size16 start() const;
		Size16 end() const;
		char *startPointer() const;
		char *endPointer() const;
		char *pointer(Size16 offset) const;
		Size16 size() const;
		Size16 room() const;
		int compare(Size16 myOffset, const Block *block, Size16 offset, Size16 length) const;
		void copyFrom(const char *cString, Size16 length);
		void copyTo(char *string, Size16 offset, Size16 length);
		void destroy(Allocator *allocator);
		void pushed(Size16 length);
		void popped(Size16 length);
		void split(Allocator *allocator, Size16 offset);

		static void *allocateMemory(Allocator *allocator);
	};

	List<Block> mBlocks;
	Size mSize;

	Block *mHere;
	Size16 mPosition;
	Size mOffset;

	Buffer(Actor *actor);

	Block *allocateBlock(Block *previousBlock);
	Block *refBlock(Block *block);
	void unrefBlock(Block *block);
	Block *ensurePush(Block *block, Size length);
	void pushToBlock(const char *cString, Size length, Block *block);
	Block *blockByOffset(Size *offset);
	Block *splitBlock(Size offset);

public:
	class Iterator {
		const Buffer<mBlockSize> *mBuffer;
		Block *mBlock;
		Size16 mStart;
		Size mOffset;

	public:
		Iterator(const Buffer<mBlockSize> *buffer);
		~Iterator();

		void advance(Size length = 1);
		Size charBlockLength() const;
		const char *charBlock() const;
		const Buffer<mBlockSize> *buffer() const;
		char current() const;
		Size offset() const;
		Size left() const;
	};

	String *asString() const;
	String *asString(Size length) const;
	String *asString(Size offset, Size length) const;
	char charAt(Size offset) const;
	int compare(const char *cString) const;
	int compare(const char *cString, Size length) const;
	int compare(const String *string) const;
	int compare(const Buffer *buffer) const;
	bool equals(const char *cString) const;
	bool equals(const char *cString, Size length) const;
	bool equals(const String *string) const;
	bool equals(const Buffer *buffer) const;
	Size length() const;

	WriteBuffer<mBlockSize> *region(Size offset, Size length);

	Size16 blockSize() const;
};

}

/*** Template implementations ***/

#include <string.h>
#include <stdlib.h>

#include "allocator.h"
#include "string.h"

namespace Stateplex {

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator)
	: mStart(0), mEnd(0)
{
	mBytes = reinterpret_cast<Bytes *>(allocator->allocate(sizeof(Bytes) + mBlockSize));
	mBytes->mReferenceCount = 0;
	mBytes->mStart = mBytes->mEnd = 0;
}

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block)
	: mBytes(block->mBytes), mStart(block->mStart), mEnd(block->mEnd)
{
	mBytes->mReferenceCount++;
}

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block, Size16 offset, Size16 length)
	: mBytes(block->mBytes), mStart(block->mStart + offset), mEnd(block->mStart + offset + length)
{
	mBytes->mReferenceCount++;
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::destroy(Allocator *allocator)
{
	mBytes->mReferenceCount--;
	if (mBytes->mReferenceCount == 0)
		Dispatcher::current()->allocator()->deallocate(mBytes, sizeof(Bytes) + mBlockSize);

	allocator->deallocate(this, sizeof(Block));
}

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::start() const
{
	return mStart;
}

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::end() const
{
	return mEnd;
}

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::startPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart;
}

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::endPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mEnd;
}

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::pointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart + offset;
}

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::size() const
{
	return mEnd - mStart;
}

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::room() const
{
	return mEnd == mBytes->mEnd ? mBlockSize - mEnd : 0;
}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::Block::compare(Size16 myOffset, const Block *block, Size16 offset, Size16 length) const
{
	return memcmp(pointer(myOffset), block->pointer(offset), length);
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::copyFrom(const char *cString, Size16 length)
{
	memcpy(endPointer(), cString, length);
	mBytes->mEnd = mEnd += length;
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::copyTo(char *cString, Size16 offset, Size16 length)
{
	memcpy(cString, startPointer() + offset, length);
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::pushed(Size16 length)
{
	mEnd += length;
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::popped(Size16 length)
{
	mStart += length;
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::split(Allocator *allocator, Size16 offset)
{
	void *memory = Block::allocateMemory(allocator);
	Block *block = new(memory) Block(allocator, this, offset, size() - offset);
	block->addAfter(this);
	mEnd = mStart + offset;
}

template<Size16 mBlockSize>
void *Buffer<mBlockSize>::Block::allocateMemory(Allocator *allocator)
{
	return allocator->allocate(sizeof(Block));
}


template<Size16 mBlockSize>
typename Buffer<mBlockSize>::Block *Buffer<mBlockSize>::allocateBlock(Block *previousBlock)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	void *memory = Block::allocateMemory(allocator);
	Block *block = new(memory) Block(allocator);
	if (previousBlock)
		block->addAfter(previousBlock);
	else
		mBlocks.addHead(block);

	if (!mHere) {
		mHere = block;
		mPosition = 0;
		mOffset = 0;
	}

	return block;
}

template<Size16 mBlockSize>
typename Buffer<mBlockSize>::Block *Buffer<mBlockSize>::ensurePush(Block *block, Size length)
{
	if (!block || length < block->room())
		allocateBlock(block);
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::pushToBlock(const char *cString, Size length, Block *block)
{
	block = ensurePush(block, 1);
	Size16 room = block->room();
	if (length < room) {
		block->copyFrom(cString, length);
		mSize += length;
	} else {
		block->copyFrom(cString, room);
		mSize += room;

		pushToBlock(cString + room, length - room, block);
	}
}

template<Size16 mBlockSize>
typename Buffer<mBlockSize>::Block *Buffer<mBlockSize>::blockByOffset(Size *offset)
{
	for (Block *block = mBlocks.first(); block; block = mBlocks.next(block)) {
		Size16 size = block->size();
		if (*offset < size)
			return block;
		*offset -= size;
	}

	return 0;
}

template<Size16 mBlockSize>
typename Buffer<mBlockSize>::Block *Buffer<mBlockSize>::splitBlock(Size offset)
{
	if (offset == 0)
		return 0;

	Allocator *allocator = actor()->dispatcher()->allocator();
	for (Block *block = mBlocks.first(); block; block = mBlocks.next(block)) {
		Size16 size = block->size();
		if (offset <= size) {
			if (offset > 0 && offset < size)
				block->split(allocator, offset);
			return block;
		}
		offset -= size;
	}

	return 0;
}

template<Size16 mBlockSize>
Buffer<mBlockSize>::Buffer(Actor *actor)
	: Object(actor), mSize(0), mHere(0), mPosition(0), mOffset(0)
{ }

/**
 * Returns the amount of bytes in the buffer.
 * 
 * @return total size of the buffer.
 */
 
template<Size16 mBlockSize>
Size Buffer<mBlockSize>::length() const
{
	return mSize;
}

/**
 * Returns the character at the specified position of the buffer.
 */

template<Size16 mBlockSize>
char Buffer<mBlockSize>::charAt(Size offset) const
{
	for (ListIterator<Block> iterator(&mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		Block *block = iterator.current();
		Size16 size = block->size();
		if (offset < size)
			return *block->pointer(offset);
		offset -= size;
	}

	return 0;
}

/**
 * Returns the contents of the buffer as a string.
 *
 * Note! This function allocates memory and copies the byte data into it.
 */
template<Size16 mBlockSize>
String *Buffer<mBlockSize>::asString() const
{
	String *string = String::uninitialised(allocator(), mSize);
	char *p = string->chars();
	for (Block *block = mBlocks.first(); block; block = mBlocks.next(block)) {
		Size16 size = block->size();
		block->copyTo(p, 0, size);
		p += size;
	}

	return string;
}

/**
 * Returns the specified amount of bytes from the beginning of the buffer as a string.
 *
 * Note! This function allocates memory and copies the byte data into it.
 */
template<Size16 mBlockSize>
String *Buffer<mBlockSize>::asString(Size length) const
{
	return asString(0, length);
}

/**
 * Returns the specified amount of bytes from the specified position of the buffer as a string.
 *
 * Note! This function allocates memory and copies the byte data into it.
 */
template<Size16 mBlockSize>
String *Buffer<mBlockSize>::asString(Size offset, Size length) const
{
	String *string = String::uninitialised(allocator(), mSize);
	char *p = string->chars();
	for (Block *block = blockByOffset(&offset); block; block = mBlocks.next(block)) {
		Size16 size = block->size();
		if (offset + length < size) {
			block->copyTo(p, offset, length);
			p += length;
			break;
		} else {
			block->copyTo(p, offset, size);
			p += size;
		}
		offset = 0;
		length -= size;
	}

	return string;
}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const char *cString) const
{

}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const char *cString, Size length) const
{

}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const String *string) const
{

}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const Buffer *buffer) const
{

}

template<Size16 mBlockSize>
bool Buffer<mBlockSize>::equals(const char *cString) const
{
	return compare(cString) == 0;
}
template<Size16 mBlockSize>
bool Buffer<mBlockSize>::equals(const char *cString, Size length) const
{
	return compare(cString, length) == 0;
}

template<Size16 mBlockSize>
bool Buffer<mBlockSize>::equals(const String *string) const
{
	return compare(string) == 0;
}

template<Size16 mBlockSize>
bool Buffer<mBlockSize>::equals(const Buffer *buffer) const
{
	return compare(buffer) == 0;
}

/**
 * Returns the specified fragment of the buffer.
 */
template<Size16 mBlockSize>
WriteBuffer<mBlockSize> *Buffer<mBlockSize>::region(Size offset, Size length)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	WriteBuffer<mBlockSize> *buffer = new WriteBuffer<mBlockSize>(actor());
	for (Block *oldBlock = blockByOffset(&offset); oldBlock; oldBlock = mBlocks.next(oldBlock)) {
		Size16 size = oldBlock->size();
		if (offset + length < size) {
			void *memory = Block::allocateMemory(allocator);
			Block *newBlock = new(memory) Block(allocator, oldBlock, offset, length);
			buffer->mBlocks.addTail(newBlock);
			buffer->mSize += length;
			break;
		} else {
			void *memory = Block::allocateMemory(allocator);
			Block *newBlock = new(memory) Block(allocator, oldBlock, offset, size);
			buffer->mBlocks.addTail(newBlock);
			buffer->mSize += size;
		}
		offset = 0;
		length -= size;
	}

	return buffer;
}

/**
 * Returns the internal block size for the buffer. This is usually only
 * required in some usage scenarios of the popPointer() in the ReadBuffer and
 * pushPointer() in the WriteBuffer.
 */
template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::blockSize() const
{
	return mBlockSize;
}

template<Size16 mBlockSize>
Buffer<mBlockSize>::Iterator::Iterator(const Buffer<mBlockSize> *buffer)
	: mBuffer(buffer), mBlock(mBuffer->mBlocks.first()), mStart(0), mOffset(0)
{
	if (mBlock)
		mStart = mBlock->start();
}

template<Size16 mBlockSize>
Buffer<mBlockSize>::Iterator::~Iterator()
{ }

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Iterator::advance(Size length)
{
	if (!mBlock)
		return;

	if (length >= left()) {

	}

	while (mBlock && mStart + length > mBlock->end()) {
		mBlock = mBuffer->mBlocks.next(mBlock);
		length -= mBlock->end() - mStart;
	}

	mStart += length;
}

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::Iterator::charBlockLength() const
{
	return mBlock->end() - mStart;
}

template<Size16 mBlockSize>
const char *Buffer<mBlockSize>::Iterator::charBlock() const
{
	return mBlock->pointer(mStart);
}

template<Size16 mBlockSize>
const Buffer<mBlockSize> *Buffer<mBlockSize>::Iterator::buffer() const
{
	return mBuffer;
}

template<Size16 mBlockSize>
char Buffer<mBlockSize>::Iterator::current() const
{
	if (!mBlock || mStart >= mBlock->end())
		return 0;

	return *mBlock->pointer(mStart);
}

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::Iterator::offset() const
{
	return mOffset;
}

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::Iterator::left() const
{
	return mBuffer->mSize - mOffset;
}

}

#endif
