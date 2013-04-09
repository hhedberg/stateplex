/*
 * Stateplex - A server-side actor model library.
 *
 * core/allocator.h
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

#ifndef INCLUDED_STATEPLEX_ALLOCATOR_H
#define INCLUDED_STATEPLEX_ALLOCATOR_H

#include "types.h"
#include "list.h"

namespace Stateplex {

/** 
 * @brief Used for allocating/deallocating memory.
 */
 
class Allocator {
	struct Slice : public ListItem {
	};

	struct Block : public ListItem {
		Allocator *mAllocator;
		Size16 mNAllocations;
	};

	/* Adjust slizeSize[To,From]Index() accordingly. */
	static const Size BLOCK_SIZE = 4 * 4096;
	static const Size MAX_SLICE_INDEX = 16 + ((BLOCK_SIZE - sizeof(Block)) / 8 - 128) / 128;

	List<Block> mFreeBlocks;
	List<Slice> mFreeSlices[MAX_SLICE_INDEX + 1];

	Size16 sliceSizeToIndex(Size size) const;
	Size sliceSizeFromIndex(Size16 index) const;
	Block *blockFromAllocation(void *memory) const;
	void allocateBlocks();
	void sliceBlock(Size16 index);
	void *allocateSlice(Size16 index);
	void *allocateChunk(Size size);
	void deallocateSlice(void *memory, Size16 index);
	void deallocateChunk(void *memory, Size size);

public:
	Allocator();
	virtual ~Allocator();

	void *allocate(Size size);
	void deallocate(void *memory, Size size);
};

}

#endif
