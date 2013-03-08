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

#include "bitset.h"
#include "types.h"
#include "list.h"

class Allocator;

void *operator new(size_t size, Allocator *allocator);
void operator delete(void *pointer, Allocator *allocator);

namespace Stateplex {

/** 
 * @brief Class Allocator
 *
 * Class Allocator is used for allocating/deallocating memory.
 */
 
template<typename T, Size multiplier = 1, Size allocationsPerBlock = 8192>
class Allocator {
	struct Block : public ListItem<Block> {
		int mAllocatedCount;
		Bitset<allocationsPerBlock> mMap;
	};

	List<Block> mBlocks;

	Block *allocateNewBlock();
	T *allocate(Block *block);

public:
	Allocator();

	T *allocate();
	void deallocate(T *allocation);
	Size size() const;
};

}

/*** Template implementations ***/

#include <exception>
#include <sys/mman.h>
#include <stdlib.h>
#include "bitset.h"

/** 
 * Overriding of new operator.
 *
 * @param size			size limit for memory allocation.
 * @param *allocator	allocator object that handles allocation.
 * @return				pointer to allocated object.
 */

void *operator new(size_t size, Stateplex::Allocator *allocator)
{
	if (allocator->size() < size)
		throw std::bad_alloc();
	return allocator->allocate();
}

/** 
 * Overriding of delete operator.
 *
 * @param *pointer		pointer to the object to be deallocated.
 * @param *allocator	allocator object that handles deallocation.
 */

void operator delete(void *pointer, Stateplex::Allocator *allocator)
{
	allocator->deallocate(pointer);
}


namespace Stateplex {

/** 
 * Constructor for Allocator class.
 *
 */

template<typename T, Size multiplier, Size allocationsPerBlock>
Allocator<T, multiplier, allocationsPerBlock>::Allocator()
{ }

/** 
 * Template function that does the actual allocation of memory.
 *
 * @return	pointer to allocated object.
 */

template<typename T, Size multiplier, Size allocationsPerBlock>
T *Allocator<T, multiplier, allocationsPerBlock>::allocate()
{
	for (ListItem<Block> item = mBlocks.first(); item; item = mBlocks.net(item)) {
		Block *block = static_cast<Block *>(item);
		if (block->mAllocatedCount < allocationsPerBlock)
			return allocate(block);
	}
	Block *block = allocateNewBlock();
	return allocate(block);
}

/** 
 * Template function that allocates new Block and returns it
 * to the caller.
 *
 * @return	returns pointer to newly allocated Block
 */

template<typename T, Size multiplier, Size allocationsPerBlock>
typename Allocator<T, multiplier, allocationsPerBlock>::Block *Allocator<T, multiplier, allocationsPerBlock>::allocateNewBlock()
{
	size_t length = sizeof(Block) + allocationsPerBlock * (sizeof(Block *) + sizeof(T)) * multiplier;
	void *addr = mmap(0, length, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED)
		::exit(100);

	Block *block = reinterpret_cast<Block *>(addr);
	mBlocks.addHead(block);
	return block;
}

/** 
 * Template function that is used for memory allocation. Is called by
 * public function allocate().
 *
 * @param *block	pointer to block that is used for allocation.
 * @return			allocated block.
 */

template<typename T, Size multiplier, Size allocationsPerBlock>
T *Allocator<T, multiplier, allocationsPerBlock>::allocate(Block *block)
{
	int allocationNumber = block->mMap.findSetBit();
	block->mAllocatedCount++;
	char *pointer = reinterpret_cast<char *>(block);
	pointer += sizeof(Block) + allocationNumber * (sizeof(Block *) + sizeof(T)) * multiplier;
	*reinterpret_cast<Block *>(pointer) = block;
	pointer += sizeof(Block *);
	return reinterpret_cast<T *>(pointer);
}

/** 
 * Template function that handles memory deallocation.
 *
 * @param *allocation	pointer to object that is going to be deallocated.
 */

template<typename T, Size multiplier, Size allocationsPerBlock>
void Allocator<T, multiplier, allocationsPerBlock>::deallocate(T *allocation)
{
	char *pointer = reinterpret_cast<char *>(allocation);
	pointer -= sizeof(Block *);
	Block *block = reinterpret_cast<Block *>(pointer);
	pointer -= block;
	int allocationNumber = pointer - sizeof(Block) - (sizeof(Block *) + sizeof(T)) * multiplier;
	if (block->mMap.isBitSet(allocationNumber))
		exit(99);
	block->mMap.unsetBit(allocationNumber);
	block->mAllocatedCount--;
}

}

#endif
