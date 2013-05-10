/*
 * Stateplex - A server-side actor model library.
 *
 * core/allocator.cpp
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

#include <stdlib.h>
#include <new>

#include "allocator.h"

namespace Stateplex {

Allocator::Allocator()
{ }

Allocator::~Allocator()
{
	/* TODO
	 * for (ListIterator<Block> iterator(&mFreeBlocks); iterator.hasCurrent(); iterator.subsequent())
	 *	free(iterator.current());
	 * for (Block *block = mFreeBlocks.first(); block; block = mFreeBlocks.next(block))
	 *	free(block);
	 */
}

Size16 Allocator::sliceSizeToIndex(Size size) const
{
	Size16 index;

	if (size < sizeof(Slice))
		size = sizeof(Slice);

	size -= sizeof(Slice);
	if (size <= 8 << 4)
		index = (size + 0xf) >> 4;
	else
		index = ((size + 0x7f) >> 7) + 7;

	return index;
}

Size Allocator::sliceSizeFromIndex(Size16 index) const
{
	Size size;

	if (index <= 8)
		size = (index) << 4;
	else
		size = (index - 7) << 7;

	return size += sizeof(Slice);
}

Allocator::Block *Allocator::blockFromAllocation(void *memory) const
{
	return reinterpret_cast<Block *>(reinterpret_cast<Size>(memory) & ~(BLOCK_SIZE - 1));
}

void Allocator::allocateBlocks()
{
	const int nBlocks = 16;

	char *memory;
	if (posix_memalign(reinterpret_cast<void **>(&memory), BLOCK_SIZE, nBlocks * BLOCK_SIZE) != 0)
		abort();

	for (int i = 0; i < nBlocks; i++) {
		Block *block = new(memory) Block();
		mFreeBlocks.addTail(block);
		memory += BLOCK_SIZE;
	}
}

void Allocator::sliceBlock(Size16 index)
{
	if (mFreeBlocks.isEmpty())
		allocateBlocks();

	Block *block = mFreeBlocks.first();
	block->remove();
	Size size = sliceSizeFromIndex(index);
	int nSlices = (BLOCK_SIZE - sizeof(Block)) / size;
	char *memory = reinterpret_cast<char *>(block) + sizeof(Block);

	List<Slice> *slices = &mFreeSlices[index];
	for (int i = 0; i < nSlices; i++) {
		Slice *slice = new(memory) Slice();
		slices->addTail(slice);
		memory += size;
	}
}

void *Allocator::allocate(Size size)
{
	if (size > (BLOCK_SIZE - sizeof(Block)) / 8)
		return allocateChunk(size);

	return allocateSlice(sliceSizeToIndex(size));
}

void *Allocator::allocateChunk(Size size)
{
	/* TODO */
	return 0;
}

/*
 * Allocates slice.
 *  
 * @return        allocated slice.
 */

void *Allocator::allocateSlice(Size16 index)
{
	List<Slice> *slices = &mFreeSlices[index];
	if (slices->isEmpty())
		sliceBlock(index);

	Slice *slice = slices->first();
	slice->remove();

	Block *block = blockFromAllocation(slice);
	if (block->mAllocator == this)
		block->mNAllocations++;

	return slice;
}

/*
 * Deallocate the allocated slice.
 */
 
void Allocator::deallocate(void *memory, Size size)
{
	if (size > (BLOCK_SIZE - sizeof(Block)) / 8)
		return deallocateChunk(memory, size);

	return deallocateSlice(memory, sliceSizeToIndex(size));
}

void Allocator::deallocateChunk(void *memory, Size size)
{
	/* TODO */
}

/*
 *
 */

void Allocator::deallocateSlice(void *memory, Size16 index)
{
	Block *block = blockFromAllocation(memory);
	Slice *slice = new(memory) Slice();
	if (block->mAllocator == this) {
		block->mNAllocations--;
		if (block->mNAllocations == 0) {
			mFreeBlocks.addHead(block);
		} else {
			mFreeSlices[index].addTail(slice);
		}
	} else {
		mFreeSlices[index].addHead(slice);
	}
}

}
