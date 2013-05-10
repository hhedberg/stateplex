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
template<Size16 blockSize> class GenericReadBuffer;
template<Size16 blockSize> class GenericWriteBuffer;

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
class GenericBuffer : public Object {
	friend class GenericReadBuffer<mBlockSize>;
	friend class GenericWriteBuffer<mBlockSize>;

	class Block;
public:
	class Iterator : public ListItem {
		friend class GenericReadBuffer<mBlockSize>;

		GenericBuffer<mBlockSize> *mBuffer;
		Block *mBlock;
		Size16 mPosition;

		bool ensureBlock();
		void deleteBlock(Block *block);

	public:
		Iterator(GenericBuffer<mBlockSize> *buffer);
		~Iterator();

		void advance(Size length = 1);
		Size offset();
		Size charBlockLength();
		const char *charBlock();
		GenericBuffer<mBlockSize> *buffer() const;
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

	GenericBuffer(Actor *actor);

	Block *blockByOffset(Size *offset);
	Size blockOffset(Block *block);
	Size doSplit(char delimiter, GenericWriteBuffer<mBlockSize> **elements, Size maxElements);

public:
	String *asString() const;
	String *asString(Size length) const;
	String *asString(Size offset, Size length) const;
	char charAt(Size offset) const;
	int compare(const char *cString) const;
	int compare(const char *cString, Size length) const;
	int compare(const String *string) const;
	int compare(const GenericBuffer *buffer) const;
	bool equals(const char *cString) const;
	bool equals(const char *cString, Size length) const;
	bool equals(const String *string) const;
	bool equals(const GenericBuffer *buffer) const;
	Size length() const;
	Size offsetOf(char c, Size fromOffset = 0);

	GenericWriteBuffer<mBlockSize> *region(Size offset, Size length);
	void region(Size offset, Size length, GenericWriteBuffer<mBlockSize> *buffer);
	Array<GenericWriteBuffer<mBlockSize> *> *split(char delimiter, Size maxElements);
	Size split(char delimiter, Array<GenericWriteBuffer<mBlockSize> *> *elements);

	Size16 blockSize() const;
};

typedef GenericBuffer<1024> Buffer;

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
 * @param *allocator	pointer to Allocator.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::Block::Block(Allocator *allocator)
	: mStart(0), mEnd(0)
{
	mBytes = reinterpret_cast<Bytes *>(allocator->allocate(sizeof(Bytes) + mBlockSize));
	mBytes->mReferenceCount = 1;
	mBytes->mStart = mBytes->mEnd = 0;
}

/**
 * Allocates a specific block and increase reference count.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block)
	: mBytes(block->mBytes), mStart(block->mStart), mEnd(block->mEnd)
{
	mBytes->mReferenceCount++;
}

/**
 * Function that allocates a specific block size
 * and increase reference count.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::Block::Block(Allocator *allocator, Block *block, Size16 offset, Size16 length)
	: mBytes(block->mBytes), mStart(block->mStart + offset), mEnd(block->mStart + offset + length)
{
	mBytes->mReferenceCount++;
}

/**
 * Deallocate the allocated block when refernce count decreases to 0.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Block::destroy(Allocator *allocator)
{
	remove();
	mBytes->mReferenceCount--;
	if (mBytes->mReferenceCount == 0)
		Dispatcher::current()->allocator()->deallocate(mBytes, sizeof(Bytes) + mBlockSize);

	allocator->deallocate(this, sizeof(Block));
}

/**
 * Returns the start of a block.
 * 
 * @return	start of the block.
 */

template<Size16 mBlockSize>
Size16 GenericBuffer<mBlockSize>::Block::start() const
{
	return mStart;
}

/**
 * Returns the end of a block.
 * 
 * @return    end of the block.
 */

template<Size16 mBlockSize>
Size16 GenericBuffer<mBlockSize>::Block::end() const
{
	return mEnd;
}

/**
 * Returns a pointer to the start character.
 * 
 * @return	pointer to character.
 */

template<Size16 mBlockSize>
char *GenericBuffer<mBlockSize>::Block::startPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart;
}

/**
 * Returns a pointer to the end character.
 * 
 * @return	pointer to character.
 */

template<Size16 mBlockSize>
char *GenericBuffer<mBlockSize>::Block::endPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mEnd;
}

/**
 * Returns a pointer to a specified place in block
 * that has been given as parameter.
 * 
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *GenericBuffer<mBlockSize>::Block::pointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart + offset;
}

/**
 * Returns a pointer to a specified place in block
 * that has been given as parameter.
 * 
 * @return	pointer to return.
 */

template<Size16 mBlockSize>
char *GenericBuffer<mBlockSize>::Block::rawPointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + offset;
}

/**
 * Calculates the size of the block.
 * 
 * @return 	pointer to return.
 */

template<Size16 mBlockSize>
Size16 GenericBuffer<mBlockSize>::Block::size() const
{
	return mEnd - mStart;
}

/**
 * Calculates the available room.
 * 
 * @return	available room.
 */

template<Size16 mBlockSize>
Size16 GenericBuffer<mBlockSize>::Block::room() const
{
	return mEnd == mBytes->mEnd ? blockSize - mEnd : 0;
}

/**
 * Compares the content of the memory block.
 */

template<Size16 mBlockSize>
int GenericBuffer<mBlockSize>::Block::compare(Size16 myOffset, const Block *block, Size16 offset, Size16 length) const
{
	return memcmp(pointer(myOffset), block->pointer(offset), length);
}

/**
 * Copies specific block.
 *
 * @param *cString      chars to be placed.
 * @param  length	number of chars to copy.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Block::copyFrom(const char *cString, Size16 length)
{
	memcpy(endPointer(), cString, length);
	mBytes->mEnd = mEnd += length;
}

/**
 * Copy chars to specific block.
 *
 * @param *cString	chars to be placed.
 * @param  length	number of chars to copy.
 */


template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Block::copyTo(char *cString, Size16 offset, Size16 length)
{
	memcpy(cString, startPointer() + offset, length);
}

/**
 * This function takes a value and adds it to a variable that represents the
 * blocks last address space. Also this value is added to the buffer size.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Block::pushed(Size16 length)
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
void GenericBuffer<mBlockSize>::Block::popped(Size16 length)
{
	mStart += length;
}

/**
 * Splits the allocated memory.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Block::split(Allocator *allocator, Size16 offset)
{
	void *memory = Block::allocateMemory(allocator);
	Block *block = new(memory) Block(allocator, this, offset, size() - offset);
	block->addAfter(this);
	mEnd = mStart + offset;
}

/**
 * Allocates memory.
 * 
 * @return 	pointer to return.
 */

template<Size16 mBlockSize>
void *GenericBuffer<mBlockSize>::Block::allocateMemory(Allocator *allocator)
{
	return allocator->allocate(sizeof(Block));
}

/**
 * Default constructor for class GenericBuffer.
 * Initializes a new instance of buffer.
 */

template<Size16 mBlockSize>
typename GenericBuffer<mBlockSize>::Block *GenericBuffer<mBlockSize>::blockByOffset(Size *offset)
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
Size GenericBuffer<mBlockSize>::blockOffset(Block *block)
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
 * Default constructor for GenericBuffer that initializes
 * a new instance of GenericBuffer.
 *
 * @parameter *actor	pointer to the owner of this.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::GenericBuffer(Actor *actor)
	: Object(actor), mSize(0)
{ }

/**
 * Function that returns the total size of GenericBuffer.
 * 
 * @return	total size of GenericBuffer.
 */
 
template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::length() const
{
	return mSize;
}


template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::offsetOf(char c, Size fromOffset)
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
char GenericBuffer<mBlockSize>::charAt(Size offset) const
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
String *GenericBuffer<mBlockSize>::asString() const
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
String *GenericBuffer<mBlockSize>::asString(Size length) const
{
	return asString(0, length);
}

/**
 * Returns the specified amount of bytes from the specified position of the buffer as a string.
 *
 * Note! This function allocates memory and copies the byte data into it.
 */

template<Size16 mBlockSize>
String *GenericBuffer<mBlockSize>::asString(Size offset, Size length) const
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

/*
 * Compares the value of the string.
 *
 * @param *cString	chars to be compared. 
 */

template<Size16 mBlockSize>
int GenericBuffer<mBlockSize>::compare(const char *cString) const
{
	return compare(cString, strlen(cString));
}

/**
 * Compares the size and length of the blocks.
 */

template<Size16 mBlockSize>
int GenericBuffer<mBlockSize>::compare(const char *cString, Size length) const
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

/*
 * Compares this string chars to string length.
 *
 * @param *string	chars to be compared.
 */

template<Size16 mBlockSize>
int GenericBuffer<mBlockSize>::compare(const String *string) const
{
	return compare(string->chars(), string->length());
}

/**
 *
 */

template<Size16 mBlockSize>
int GenericBuffer<mBlockSize>::compare(const GenericBuffer *buffer) const
{
	// TODO
	return 0;
}

/**
 * Compares the content of string.
 *
 * @param *cString	chars to be compared.
 * @retrun		true or false.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::equals(const char *cString) const
{
	return compare(cString) == 0;
}

/*
 * Compares the the size of this string.
 *
 * @param *cString	chars to be compared.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::equals(const char *cString, Size length) const
{
	return compare(cString, length) == 0;
}

/*
 * Compares this pointer string.
 *
 * @return	    True or false.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::equals(const String *string) const
{
	return compare(string) == 0;
}

/*
 * Compares this pointer buffer.
 *
 * @return	  True or false.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::equals(const GenericBuffer *buffer) const
{
	return compare(buffer) == 0;
}

/**
 * Returns the specified fragment of the buffer as a new buffer.
 */

template<Size16 mBlockSize>
GenericWriteBuffer<mBlockSize> *GenericBuffer<mBlockSize>::region(Size offset, Size length)
{
	GenericWriteBuffer<mBlockSize> *buffer = new GenericWriteBuffer<mBlockSize>(actor());
	region(offset, length, buffer);
	return buffer;
}

/**
 * Appends the specified fragment of the buffer into other buffer.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::region(Size offset, Size length, GenericWriteBuffer<mBlockSize> *buffer)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	for (Block *oldBlock = blockByOffset(&offset); oldBlock; oldBlock = mBlocks.next(oldBlock)) {
		Size16 size = oldBlock->size();
		if (offset + length <= size) {
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
}

/**
 * Prepares elements for split.
 *
 * @return	nElements.
 */

template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::doSplit(char delimiter, GenericWriteBuffer<mBlockSize> **elements, Size maxElements)
{
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

	return nElements;
}

/*
 * Do the actual split of the elements.
 */

template<Size16 mBlockSize>
Array<GenericWriteBuffer<mBlockSize> *> *GenericBuffer<mBlockSize>::split(char delimiter, Size maxElements)
{
	GenericWriteBuffer<mBlockSize> *elements[maxElements];
	Size nElements = doSplit(delimiter, elements, maxElements);

	return Array<GenericWriteBuffer<mBlockSize> *>::copy(allocator(), elements, nElements);
}

template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::split(char delimiter, Array<GenericWriteBuffer<mBlockSize> *> *elements)
{
	return doSplit(delimiter, elements->elements(), elements->length());
}


/**
 * Returns the internal block size for the buffer. This is usually only
 * required in some usage scenarios of the popPointer() in the ReadBuffer and
 * pushPointer() in the WriteBuffer.
 */

template<Size16 mBlockSize>
Size16 GenericBuffer<mBlockSize>::blockSize() const
{
	return mBlockSize;
}

/**
 * Initialize new instance of Iterator. 
 * Add block at the end of buffer.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::Iterator::Iterator(GenericBuffer<mBlockSize> *buffer)
	: mBuffer(buffer), mBlock(0), mPosition(0)
{
	mBuffer->mIterators.addTail(this);
}

/**
 * Default destructor function.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize>::Iterator::~Iterator()
{
	remove();
}

/**
 * Function that ensures if member variable inline accordingly.
 *
 * @return	True or False.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::Iterator::ensureBlock()
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

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Iterator::advance(Size length)
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
 * Gets current position of the block.
 *
 * @return	current start of the block.
 */

template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::Iterator::offset()
{
	if (!ensureBlock())
		return mBuffer->length();

	return mBuffer->blockOffset(mBlock) + mPosition - mBlock->start();
}

/**
 * Calculates current block length.
 */

template<Size16 mBlockSize>
Size GenericBuffer<mBlockSize>::Iterator::charBlockLength()
{
	if (!ensureBlock())
		return 0;

	return mBlock->end() - mPosition;
}

/**
 * Returns rawPointer to block position.
 */

template<Size16 mBlockSize>
const char *GenericBuffer<mBlockSize>::Iterator::charBlock()
{
	if (!ensureBlock())
		return 0;

	return mBlock->rawPointer(mPosition);
}

/**
 * Returns member variable Buffer.
 */

template<Size16 mBlockSize>
GenericBuffer<mBlockSize> *GenericBuffer<mBlockSize>::Iterator::buffer() const
{
	return mBuffer;
}

/**
 * Returns the current position of block end.
 *
 * @return 	current end of the block.
 */

template<Size16 mBlockSize>
bool GenericBuffer<mBlockSize>::Iterator::hasCurrent()
{
	return ensureBlock() && mPosition < mBlock->end();
}

/**
 * Returns pointer to current position of block.
 */

template<Size16 mBlockSize>
char GenericBuffer<mBlockSize>::Iterator::current()
{
	if (!ensureBlock())
		return 0;

	return *(mBlock->rawPointer(mPosition));
}

/**
 * Delete blocks if the value is different from 0
 * and returns the start or the end of block.
 */

template<Size16 mBlockSize>
void GenericBuffer<mBlockSize>::Iterator::deleteBlock(Block *block)
{
	if (block != mBlock)
		return;

	if ((mBlock = mBuffer->mBlocks.next(mBlock)) != 0)
		mPosition = mBlock->start();
	else if ((mBlock = mBuffer->mBlocks.previous(block)) != 0)
		mPosition = mBlock->end();
}

}

#endif
