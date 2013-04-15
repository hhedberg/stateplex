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

/**
 * @brief Receives/pushes data to blocks and then consumes then.
 *
 * This makes it possible to receive/pop messages fast and then we can “consume” the data as fast as we can process it.
 * The data just stays in the buffer block for the time it takes to consume them.
 */

class Allocator;
class String;

template<Size16 blockSize = 1024>
class Buffer : public Object {
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

	Block *allocateBlock(Block *previousBlock);
	Block *refBlock(Block *block);
	void unrefBlock(Block *block);
	Block *ensurePush(Block *block, Size length);
	void pushToBlock(const char *cString, Size length, Block *block);
	Block *blockByOffset(Size *offset);
	Block *splitBlock(Size offset);

public:
	class Iterator {
		const Buffer<blockSize> *mBuffer;
		Block *mBlock;
		Size16 mStart;
		Size mOffset;

	public:
		Iterator(const Buffer<blockSize> *buffer);
		~Iterator();

		void advance(Size length = 1);
		Size charBlockLength() const;
		const char *charBlock() const;
		const Buffer<blockSize> *buffer() const;
		char current() const;
		Size left() const;
	};

	Buffer(Actor *actor);
	Buffer(Actor *actor, const char *cString);

	void append(Buffer *buffer);
	void append(const char *cString);
	void append(const char *cString, Size length);

	void ensurePushLength(Size16 length);
	char *pushPointer() const;
	Size16 pushLength() const;
	void pushed(Size16 length);

	char peek() const;
	char pop();

	const char *popPointer() const;
	Size16 popLength() const;
	void popped(Size16 length);
	void poppedAll();

	Size length() const;

	char here() const;
	void next();
	Size left() const;
	Size offset() const;
	void popHere();

	char charAt(Size offset) const;
	String *asString() const;
	String *asString(Size length) const;
	String *asString(Size offset, Size length) const;

	int compare(const char *cString) const;
	int compare(const char *cString, Size length) const;
	int compare(const String *string) const;
	int compare(const Buffer *buffer) const;
	bool equals(const char *cString) const;
	bool equals(const char *cString, Size length) const;
	bool equals(const String *string) const;
	bool equals(const Buffer *buffer) const;

	Buffer *region(Size offset, Size length);
	void insert(Size offset, Buffer *buffer);
	void insert(Size insertOffset, Buffer *buffer, Size offset, Size length);
	void insert(Size offset, const char *cString);
	void insert(Size offset, const char *cString, Size length);

	Size16 maximumPushLength() const;
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
 * @return		allocated block
 */
 
template<Size16 blockSize>
Buffer<blockSize>::Block::Block(Allocator *allocator)
	: mStart(0), mEnd(0)
{
	mBytes = reinterpret_cast<Bytes *>(allocator->allocate(sizeof(Bytes) + blockSize));
	mBytes->mReferenceCount = 0;
	mBytes->mStart = mBytes->mEnd = 0;
}

template<Size16 blockSize>
Buffer<blockSize>::Block::Block(Allocator *allocator, Block *block)
	: mBytes(block->mBytes), mStart(block->mStart), mEnd(block->mEnd)
{
	mBytes->mReferenceCount++;
}

template<Size16 blockSize>
Buffer<blockSize>::Block::Block(Allocator *allocator, Block *block, Size16 offset, Size16 length)
	: mBytes(block->mBytes), mStart(block->mStart + offset), mEnd(block->mStart + offset + length)
{
	mBytes->mReferenceCount++;
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::destroy(Allocator *allocator)
{
	mBytes->mReferenceCount--;
	if (mBytes->mReferenceCount == 0)
		Dispatcher::current()->allocator()->deallocate(mBytes, sizeof(Bytes) + blockSize);

	allocator->deallocate(this, sizeof(Block));
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::Block::start() const
{
	return mStart;
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::Block::end() const
{
	return mEnd;
}

template<Size16 blockSize>
char *Buffer<blockSize>::Block::startPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart;
}

template<Size16 blockSize>
char *Buffer<blockSize>::Block::endPointer() const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mEnd;
}

template<Size16 blockSize>
char *Buffer<blockSize>::Block::pointer(Size16 offset) const
{
	return reinterpret_cast<char *>(mBytes) + sizeof(Bytes) + mStart + offset;
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::Block::size() const
{
	return mEnd - mStart;
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::Block::room() const
{
	return mEnd == mBytes->mEnd ? blockSize - mEnd : 0;
}

template<Size16 blockSize>
int Buffer<blockSize>::Block::compare(Size16 myOffset, const Block *block, Size16 offset, Size16 length) const
{
	return memcmp(pointer(myOffset), block->pointer(offset), length);
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::copyFrom(const char *cString, Size16 length)
{
	memcpy(endPointer(), cString, length);
	mBytes->mEnd = mEnd += length;
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::copyTo(char *cString, Size16 offset, Size16 length)
{
	memcpy(cString, startPointer() + offset, length);
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::pushed(Size16 length)
{
	mEnd += length;
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::popped(Size16 length)
{
	mStart += length;
}

template<Size16 blockSize>
void Buffer<blockSize>::Block::split(Allocator *allocator, Size16 offset)
{
	void *memory = Block::allocateMemory(allocator);
	Block *block = new(memory) Block(allocator, this, offset, size() - offset);
	block->addAfter(this);
	mEnd = mStart + offset;
}

template<Size16 blockSize>
void *Buffer<blockSize>::Block::allocateMemory(Allocator *allocator)
{
	return allocator->allocate(sizeof(Block));
}


template<Size16 blockSize>
typename Buffer<blockSize>::Block *Buffer<blockSize>::allocateBlock(Block *previousBlock)
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

/**
 * Function that deallocates a specific block.
 * 
 * @param block		block to deallocate
 */
 
template<Size16 blockSize>
typename Buffer<blockSize>::Block *Buffer<blockSize>::ensurePush(Block *block, Size length)
{
	if (!block || length < block->room())
		allocateBlock(block);
}

/**
 * Function that is used to place chars to a specific block.
 * 
 * @param *cString	chars to be placed
 * @param length	number of chars to copy
 * @param *block	target block to push
 */

template<Size16 blockSize>
void Buffer<blockSize>::pushToBlock(const char *cString, Size length, Block *block)
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

/**
 * Default constructor for class Buffer.
 * Initializes a new instance of buffer.
 */

template<Size16 blockSize>
typename Buffer<blockSize>::Block *Buffer<blockSize>::blockByOffset(Size *offset)
{
	for (Block *block = mBlocks.first(); block; block = mBlocks.next(block)) {
		Size16 size = block->size();
		if (*offset < size)
			return block;
		*offset -= size;
	}

	return 0;
}

template<Size16 blockSize>
typename Buffer<blockSize>::Block *Buffer<blockSize>::splitBlock(Size offset)
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

template<Size16 blockSize>
Buffer<blockSize>::Buffer(Actor *actor)
	: Object(actor), mSize(0), mHere(0), mPosition(0), mOffset(0)
{ }

/**
 * Constructor for class Buffer.
 * This constructor has an ability to take in a string straight to buffer.
 * 
 * @param *string	string to put into the buffer.
 */

template<Size16 blockSize>
Buffer<blockSize>::Buffer(Actor *actor, const char *cString)
	: Object(actor), mSize(0), mHere(0), mPosition(0), mOffset(0)
{
	append(cString);
}

/**
 * Function that takes in another buffer and goes through
 * every block item in the buffer and adds them to this buffer.
 * 
 * @param buffer	target buffer to be added.
 */

template<Size16 blockSize>
void Buffer<blockSize>::append(Buffer *buffer)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	Block *previousBlock = mBlocks.last();
	for (ListIterator<Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = Block::allocateMemory(allocator);
		Block *block = new(memory) Block(allocator, iterator.current());
		block->addAfter(previousBlock);
		mSize += block->size();
		previousBlock = block;
	}
}

/**
 * Function that takes in a string and calls push function to place
 * the string to buffer.
 * 
 * @param *string	string to be added
 */

template<Size16 blockSize>
void Buffer<blockSize>::append(const char *cString)
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

template<Size16 blockSize>
void Buffer<blockSize>::append(const char *cString, Size length)
{
	pushToBlock(cString, length, mBlocks.last());
}

/**
 * Function that ensures push length.
 * It check that there are blocks if none allocates a new one.
 * If there are blocks it ensures that push length is not too big
 * to be allocated.
 * 
 * @param length	maximum push length.
 */

template<Size16 blockSize>
void Buffer<blockSize>::ensurePushLength(Size16 length)
{
	ensurePush(mBlocks.last(), length);
}

/**
 * Function that returns pointer to next of the last block saved.
 * 
 * @return	pointer to return.
 */

template<Size16 blockSize>
char *Buffer<blockSize>::pushPointer() const
{
	Block *block = mBlocks.last();
	return block->endPointer();
}

/**
 * Function that returns last pushed blocks length.
 * 
 * @return	lenght of the push.
 */

template<Size16 blockSize>
Size16 Buffer<blockSize>::pushLength() const
{
	Block *block = mBlocks.last();
	return block->room();
}

/**
 * Function that is called when a new block is added to buffer.
 * This function takes a value and adds it to a variable that represends the
 * blocks last address space. Also this value is added to the buffer size.
 * 
 * @param length	value to be assigned.
 */

template<Size16 blockSize>
void Buffer<blockSize>::pushed(Size16 length)
{
	Block *block = mBlocks.last();
	block->pushed(length);
	mSize += length;
}

/**
 * Function that peeks at the value of first block in buffer.
 * Calls function popPointer to get the address of the first value in
 * buffer.
 * 
 * @return	value that has been taken from address which was pointed.
 */

template<Size16 blockSize>
char Buffer<blockSize>::peek() const
{
	return *popPointer();
}

/**
 * Function that takes the first value from buffer and stores it
 * to a variable and removes the value from the buffer.
 * 
 * @return	value that was popped.
 */

template<Size16 blockSize>
char Buffer<blockSize>::pop()
{
	char c = *popPointer();
	popped(1);
	return c;
}

/**
 * Function that returns pointer to the starting point of the first block.
 * 
 * @return	pointer to return.
 */

template<Size16 blockSize>
const char *Buffer<blockSize>::popPointer() const
{
	Block *block = mBlocks.first();
	return reinterpret_cast<char *>(block) + sizeof(Block) + block->mStart;
}

/**
 * Function that returns the length of the first block.
 * 
 * @return	the size of the first block.
 */

template<Size16 blockSize>
Size16 Buffer<blockSize>::popLength() const
{
	Block *block = mBlocks.first();
	return block->mEnd - block->mStart;
}

/**
 * Function that deallocates the first block from the buffer.
 * This function is called when a value has been popped from buffer.
 *
 * @param length	length of the value that has been popped.
 */

template<Size16 blockSize>
void Buffer<blockSize>::popped(Size16 length)
{
	for (ListIterator<Block> iterator(&mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		Block *block = iterator.current();
		Size16 size = block->size();
		if (size > length) {
			block->popped(length);
			break;
		} else {
			block->remove();
			block->destroy(allocator());
			length -= size;
		}
	}
	mSize -= length;
	/* TODO: Here */
}

template<Size16 blockSize>
void Buffer<blockSize>::poppedAll()
{
	popped(this->mSize);
}


/**
 * Function that returns the total size of buffer.
 * 
 * @return	total size of buffer.
 */
 
template<Size16 blockSize>
Size Buffer<blockSize>::length() const
{
	return mSize;
}

/**
 * Function that returns the value of current position in buffer.
 * 
 * @return	value of the current position.
 */
 
template<Size16 blockSize>
char Buffer<blockSize>::here() const
{
	return *mHere->pointer(mPosition);
}

/**
 * Function that moves to the next position in the buffer.
 */

template<Size16 blockSize>
void Buffer<blockSize>::next()
{
	mPosition++;
	if (mPosition >= mHere->size()) {
		mHere = mBlocks.next(mHere);
		mPosition = 0;
	}
	mOffset++;
}

/**
 * Function that calculates how much there is left to handle in the buffer.
 * 
 * @return	value left to handle.
 */

template<Size16 blockSize>
Size Buffer<blockSize>::left() const
{
	return mSize - mOffset;
}

template<Size16 blockSize>
Size Buffer<blockSize>::offset() const
{
	return mOffset;
}

/**
 * This function deallocates every block that is behind the current position's
 * block. After this the function calls for function popped and set current position
 * in buffer to current block.
 */

template<Size16 blockSize>
void Buffer<blockSize>::popHere()
{
	for (ListItem *item = mBlocks.previous(mHere); item; item = mBlocks.previous(item)) {
		Block *block = static_cast<Block *>(item);
		mSize -= block->mEnd - block->mStart;
		block->destroy(allocator());
	}
	popped(mPosition - mHere->mStart);

	mHere = static_cast<Block *>(mBlocks.first());
	mPosition = mHere->mStart;
	mOffset = 0;
}

template<Size16 blockSize>
char Buffer<blockSize>::charAt(Size offset) const
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

template<Size16 blockSize>
String *Buffer<blockSize>::asString() const
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

template<Size16 blockSize>
String *Buffer<blockSize>::asString(Size length) const
{
	return asString(0, length);
}

template<Size16 blockSize>
String *Buffer<blockSize>::asString(Size offset, Size length) const
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

template<Size16 blockSize>
int Buffer<blockSize>::compare(const char *cString) const
{

}

template<Size16 blockSize>
int Buffer<blockSize>::compare(const char *cString, Size length) const
{

}

template<Size16 blockSize>
int Buffer<blockSize>::compare(const String *string) const
{

}

template<Size16 blockSize>
int Buffer<blockSize>::compare(const Buffer *buffer) const
{

}

template<Size16 blockSize>
bool Buffer<blockSize>::equals(const char *cString) const
{
	return compare(cString) == 0;
}
template<Size16 blockSize>
bool Buffer<blockSize>::equals(const char *cString, Size length) const
{
	return compare(cString, length) == 0;
}

template<Size16 blockSize>
bool Buffer<blockSize>::equals(const String *string) const
{
	return compare(string) == 0;
}

template<Size16 blockSize>
bool Buffer<blockSize>::equals(const Buffer *buffer) const
{
	return compare(buffer) == 0;
}

template<Size16 blockSize>
Buffer<blockSize> *Buffer<blockSize>::region(Size offset, Size length)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	Buffer *buffer = new Buffer(actor());
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

template<Size16 blockSize>
void Buffer<blockSize>::insert(Size offset, Buffer *buffer)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	Block *previousBlock = splitBlock(offset);
	for (ListIterator<Block> iterator(&buffer->mBlocks); iterator.hasCurrent(); iterator.subsequent()) {
		void *memory = Block::allocateMemory(allocator);
		Block *block = new(memory) Block(iterator.current());
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			mBlocks.addHead(block);
		mSize += block->size();
		previousBlock = block;
	}
}

template<Size16 blockSize>
void Buffer<blockSize>::insert(Size insertOffset, Buffer *buffer, Size offset, Size length)
{
	Allocator *allocator = actor()->dispatcher()->allocator();
	Block *previousBlock = splitBlock(insertOffset);
	for (Block *block = blockByOffset(&offset); block && length; block = mBlocks.next(block)) {
		Size size;
		if (offset + length < block->size())
			size = length;
		else
			size = block->size();
		void *memory = Block::allocateMemory(allocator);
		Block *newBlock = new(memory) Block(allocator, block, offset, size);
		if (previousBlock)
			block->addAfter(previousBlock);
		else
			mBlocks.addHead(block);
		mSize += size;
		length -= size;
		previousBlock = block;
	}
}

template<Size16 blockSize>
void Buffer<blockSize>::insert(Size offset, const char *cString)
{
	insert(offset, cString, strlen(cString));
}

template<Size16 blockSize>
void Buffer<blockSize>::insert(Size offset, const char *cString, Size length)
{
	Block *previousBlock = splitBlock(offset);
	pushToBlock(cString, strlen(cString), previousBlock);
}

template<Size16 blockSize>
Size16 Buffer<blockSize>::maximumPushLength() const
{
	return blockSize;
}

template<Size16 blockSize>
Buffer<blockSize>::Iterator::Iterator(const Buffer<blockSize> *buffer)
	: mBuffer(buffer), mBlock(mBuffer->mBlocks.first()), mStart(0), mOffset(0)
{
	if (mBlock)
		mStart = mBlock->start();
}

template<Size16 blockSize>
Buffer<blockSize>::Iterator::~Iterator()
{ }

template<Size16 blockSize>
void Buffer<blockSize>::Iterator::advance(Size length)
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

template<Size16 blockSize>
Size Buffer<blockSize>::Iterator::charBlockLength() const
{
	return mBlock->end() - mStart;
}

template<Size16 blockSize>
const char *Buffer<blockSize>::Iterator::charBlock() const
{
	return mBlock->pointer(mStart);
}

template<Size16 blockSize>
const Buffer<blockSize> *Buffer<blockSize>::Iterator::buffer() const
{
	return mBuffer;
}

template<Size16 blockSize>
char Buffer<blockSize>::Iterator::current() const
{
	if (!mBlock || mStart >= mBlock->end())
		return 0;

	return *mBlock->pointer(mStart);
}

template<Size16 blockSize>
Size Buffer<blockSize>::Iterator::left() const
{
	return mBuffer->mSize - mOffset;
}

}

#endif
