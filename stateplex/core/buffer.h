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
#include "array.h"
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

	class Block;
public:
	class Iterator : public ListItem {
		friend class ReadBuffer<mBlockSize>;

		Buffer<mBlockSize> *mBuffer;
		Block *mBlock;
		Size16 mPosition;

		bool ensureBlock();
		void deleteBlock(Block *block);

	public:
		Iterator(Buffer<mBlockSize> *buffer);
		~Iterator();

		void advance(Size length = 1);
		Size offset();
		Size charBlockLength();
		const char *charBlock();
		Buffer<mBlockSize> *buffer() const;
		bool hasCurrent();
		char current();
	};

private:
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
		char *rawPointer(Size16 offset) const;
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
	List<Iterator> mIterators;

	Buffer(Actor *actor);

	Block *blockByOffset(Size *offset);
	Size blockOffset(Block *block);

public:
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
	Size offsetOf(char c, Size fromOffset = 0);

	WriteBuffer<mBlockSize> *region(Size offset, Size length);
	Array<WriteBuffer<mBlockSize> *> *split(char delimiter, Size maxElements);

	Size16 blockSize() const;
};

}

/*** Template implementations ***/

#include <string.h>
#include <stdlib.h>

#include "allocator.h"
#include "string.h"

namespace Stateplex {

/**
 * Function that allocates a block.
 *
 * @return		allocated block.
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator)
	: mStart(0), mEnd(0)
{
	mBytes = reinterpret_cast<Bytes *>(allocator->allocate(sizeof(Bytes) + mBlockSize));
	mBytes->mReferenceCount = 0;
	mBytes->mStart = mBytes->mEnd = 0;
}

/**
 * Function that allocates a specific block.
 *
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block)
	: mBytes(block->mBytes), mStart(block->mStart), mEnd(block->mEnd)
{
	mBytes->mReferenceCount++;
}

/**
 * Function that allcoates a specific block size.
 *
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block, Size16 offset, Size16 length)
	: mBytes(block->mBytes), mStart(block->mStart + offset), mEnd(block->mStart + offset + length)
{
	mBytes->mReferenceCount++;
}

/**
 * Fucntion that destroy the allocated block if refernce count is 0.
 *
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::destroy(Allocator *allocator)
{
	remove();
	mBytes->mReferenceCount--;
	if (mBytes->mReferenceCount == 0)
		Dispatcher::current()->allocator()->deallocate(mBytes, sizeof(Bytes) + mBlockSize);

	allocator->deallocate(this, sizeof(Block));
}

/**
 * Function that starts a block.
 *
 * @return   member variable Start.
 */

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::start() const
{
	return mStart;
}

/**
 * Function that end a block.
 *
 * @return    member variable End.
 */

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::end() const
{
	return mEnd;
}

/**
 * Function that start pointer.
 *
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::startPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart;
}

/**
 * Function that end pointer.
 *
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::endPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mEnd;
}

/**
 * Function that takes specific size and offset.
 *
 * @return
 */
template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::pointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart + offset;
}

/**
 * Function that calculates the size of the block.
 *
 * @return 	allocated size.
 */

template<Size16 mBlockSize>
char *Buffer<mBlockSize>::Block::rawPointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + offset;
}

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::size() const
{
	return mEnd - mStart;
}

/**
 * Function that calculates available room.
 *
 * @return	available room.
 */

template<Size16 mBlockSize>
Size16 Buffer<mBlockSize>::Block::room() const
{
	return mEnd == mBytes->mEnd ? mBlockSize - mEnd : 0;
}

/**
 * Function that compare the content of the memory block.
 *
 * @return	int.
 */

template<Size16 mBlockSize>
int Buffer<mBlockSize>::Block::compare(Size16 myOffset, const Block *block, Size16 offset, Size16 length) const
{
	return memcmp(pointer(myOffset), block->pointer(offset), length);
}

/**
 * Function that is used to copy specific block.
 *
 * @param *cString      chars to be placed.
 * @param  length	number of chars to copy.
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::copyFrom(const char *cString, Size16 length)
{
	memcpy(endPointer(), cString, length);
	mBytes->mEnd = mEnd += length;
}

/**
 * Function that is used to copy chars to specific block.
 *
 * @param *cString	chars to be placed.
 * @param  length	number of chars to copy.
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::copyTo(char *cString, Size16 offset, Size16 length)
{
	memcpy(cString, startPointer() + offset, length);
}

/**
 * This function takes a value and adds it to a variable that  	 represents the
 * blocks last address space. Also this value is added to the buffer size.
 *
 */
template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::pushed(Size16 length)
{
	mEnd += length;
}

/**
 * This function takes a value and adds it to a variable that represents the
 * blocks begining address space. Also this value is added to the buffer size.
 *
 * @param length	number of chars to popped.
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::popped(Size16 length)
{
	mStart += length;
}

/**
 * Function that split the allocated memory.
 *
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Block::split(Allocator *allocator, Size16 offset)
{
	void *memory = Block::allocateMemory(allocator);
	Block *block = new(memory) Block(allocator, this, offset, size() - offset);
	block->addAfter(this);
	mEnd = mStart + offset;
}

/**
 * Function that allocates memory.
 *
 * @return 	pointer to return.
 */

template<Size16 mBlockSize>
void *Buffer<mBlockSize>::Block::allocateMemory(Allocator *allocator)
{
	return allocator->allocate(sizeof(Block));
}

/**
 * Default constructor for class Buffer.
 * Initializes a new instance of buffer.
 */

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

/*
 * Return the current postion of the block
 * otherwise return the previous offset.
 *
 */

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::blockOffset(Block *block)
{
	Size offset = 0;
	while (1) {
		block = mBlocks.previous(block);
		if (!block)
			return offset;
		offset += block->size();
	}
}

/**
 * Default constructor for buffer.
 *
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Buffer(Actor *actor)
	: Object(actor), mSize(0)
{ }

/**
 * Returns the amount of bytes in the buffer.
 * 
 * @return 	total size of the buffer.
 */
 
template<Size16 mBlockSize>
Size Buffer<mBlockSize>::length() const
{
	return mSize;
}

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::offsetOf(char c, Size fromOffset)
{
	for (Block *block = blockByOffset(&fromOffset); block; block = mBlocks.next(block), fromOffset = 0) {
		char *found = reinterpret_cast<char *>(memchr(block->pointer(fromOffset), c, block->size() - fromOffset));
		if (found) {
			return found - block->startPointer(); // TODO: take block offset into account
		}
	}
	return SIZE_ERROR;
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
	return compare(cString, strlen(cString));
}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const char *cString, Size length) const
{
	if (length > mSize)
		return 1;

	int result = 0;
	for (Block *block = mBlocks.first(); length > 0; block = mBlocks.next(block)) {
		if (block->size() > length)
			return memcmp(cString, block->startPointer(), length);
		result = memcmp(cString, block->startPointer(), block->size());
		if (result != 0)
			return result;
		length -= block->size();
		cString += block->size();
	}

	return 0;
}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const String *string) const
{
	return compare(string->chars(), string->length());
}

template<Size16 mBlockSize>
int Buffer<mBlockSize>::compare(const Buffer *buffer) const
{
	// TODO
	return 0;
}

/**
 * Function that compare content of the memory block if string is equal to 0.
 *
 *
 */

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

template<Size16 mBlockSize>
Array<WriteBuffer<mBlockSize> *> *Buffer<mBlockSize>::split(char delimiter, Size maxElements)
{
	WriteBuffer<mBlockSize> *elements[maxElements];
	Size nElements = 0;

	Size previousOffset = 0;
	Size currentOffset;
	while (previousOffset < length() && (currentOffset = offsetOf(delimiter, previousOffset)) < length() && nElements < maxElements - 1) {
		elements[nElements++] = region(previousOffset, currentOffset - previousOffset);
		previousOffset = currentOffset + 1;
	}

	if (previousOffset < length()) {
		elements[nElements++] = region(previousOffset, length() - previousOffset);
	}

	return Array<WriteBuffer<mBlockSize> *>::copy(allocator(), elements, nElements);
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

/**
 * Add block at the end of buffer.
 *
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Iterator::Iterator(Buffer<mBlockSize> *buffer)
	: mBuffer(buffer), mBlock(0), mPosition(0)
{
	mBuffer->mIterators.addTail(this);
}

/**
 * Destructor function.
 *
 */

template<Size16 mBlockSize>
Buffer<mBlockSize>::Iterator::~Iterator()
{
	remove();
}

/**
 * Function that ensures if the block is in the right position or not.
 *
 * @return	True or False.
 */

template<Size16 mBlockSize>
bool Buffer<mBlockSize>::Iterator::ensureBlock()
{
	if (!mBlock) {
		if ((mBlock = mBuffer->mBlocks.first()) != 0) {
			mPosition = mBlock->start();
			return true;
		}
		return false;
	}

	if (mPosition < mBlock->start())
		mPosition = mBlock->start();

	return true;
}

/*
 *
 *
 */

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Iterator::advance(Size length)
{
	if (!ensureBlock())
		return;

	while (mPosition + length >= mBlock->end()) {
		length -= mBlock->end() - mPosition;
		Block *block = mBuffer->mBlocks.next(mBlock);
		if (!block) {
			mPosition = mBlock->end();
			return;
		}
		mBlock = block;
		mPosition = mBlock->start();
	}

	mPosition += length;
}

/*
 * Gets current postion of the block.
 *
 */

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::Iterator::offset()
{
	if (!ensureBlock())
		return mBuffer->length();

	return mBuffer->blockOffset(mBlock) + mPosition - mBlock->start();
}

template<Size16 mBlockSize>
Size Buffer<mBlockSize>::Iterator::charBlockLength()
{
	if (!ensureBlock())
		return 0;

	return mBlock->end() - mPosition;
}

template<Size16 mBlockSize>
const char *Buffer<mBlockSize>::Iterator::charBlock()
{
	if (!ensureBlock())
		return 0;

	return mBlock->rawPointer(mPosition);
}

template<Size16 mBlockSize>
Buffer<mBlockSize> *Buffer<mBlockSize>::Iterator::buffer() const
{
	return mBuffer;
}

template<Size16 mBlockSize>
bool Buffer<mBlockSize>::Iterator::hasCurrent()
{
	return ensureBlock() && mPosition < mBlock->end();
}

template<Size16 mBlockSize>
char Buffer<mBlockSize>::Iterator::current()
{
	if (!ensureBlock())
		return 0;

	return *(mBlock->rawPointer(mPosition));
}

template<Size16 mBlockSize>
void Buffer<mBlockSize>::Iterator::deleteBlock(Block *block)
{
	if (block != mBlock)
		return;

	if ((mBlock = mBuffer->mBlocks.next(mBlock)) != 0 ||
		(mBlock = mBuffer->mBlocks.last()) != 0) {
		mPosition = mBlock->start();
	}
}

}

#endif
