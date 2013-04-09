/*
 * Stateplex - A server-side actor model library.
 *
 * core/bitset.h
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

#ifndef INCLUDED_STATEPLEX_BITSET_H
#define INCLUDED_STATEPLEX_BITSET_H

#include "types.h"

namespace Stateplex {

#define STATEPLEX_BITSET_WORD_SIZE ((numberOfBits - 1) / sizeof(Word) + 1)

/**
 * @brief Used to set specific bits to different values.
 *
 * Bitset class can also be used to search for bits that are set. This class can 
 * also be used to control the allocation of memory.
 */

template<Size numberOfBits>
class Bitset {
	Word bits[STATEPLEX_BITSET_WORD_SIZE];

public:
	Bitset(bool setBits = false);
	Size size();
	void setBit(Size bit);
	void unsetBit(Size bit);
	bool isBitSet(Size bit);
	Size findSetBit();
};

}

/*** Inline implementations ***/

#include <string.h>

namespace Stateplex {

/**
 * Constructor for class Bitset.
 * The constructor initializes a new instance of class bitset.
 * Also fills it's memory space bits with ones or zeros.
 * 
 * @param setBits	boolean that determines the values for bits
 */

template<Size numberOfBits>
inline Bitset<numberOfBits>::Bitset(bool setBits)
{
	memset(bits, setBits ? 1 : 0, STATEPLEX_BITSET_WORD_SIZE * sizeof(Word));
}

/**
 * Function that is used to limit the maximum allocation size
 * in Allocator class.
 * 
 * @return 		limit for the Allocator.
 */

template<Size numberOfBits>
inline Size Bitset<numberOfBits>::size()
{
	return STATEPLEX_BITSET_WORD_SIZE * sizeof(Word);
}

/**
 * Function that sets specific bit to one.
 * 
 * @param bit		target bit to set.
 */

template<Size numberOfBits>
inline void Bitset<numberOfBits>::setBit(Size bit)
{
	bits[bit / sizeof(Word)] |= 1 << (bit % sizeof(Word));
}
	
/**
 * Function that unsets specific bit.
 * 
 * @param bit		target bit to unset.
 */
	
template <Size numberOfBits>
void Bitset<numberOfBits>::unsetBit(Size bit)
{
	bits[bit / sizeof(Word)] &= ~(1 << (bit % sizeof(Word)));
}

/**
 * Function that checks if specific bit is set.
 * 
 * @param bit		target bit to check.
 * @return		true if set, otherwise false.
 */

template <Size numberOfBits>
bool Bitset<numberOfBits>::isBitSet(Size bit)
{
	return bits[bit / sizeof(Word)] & (1 << (bit % sizeof(Word)));
}


/**
 * Function that checks for first set bit.
 * 
 * @return		numberOfBits if bits are not set, else return place of first
 * 			set bit.
 */

template <Size numberOfBits>
Size Bitset<numberOfBits>::findSetBit()
{
	for (Word *p = bits; p < bits + STATEPLEX_BITSET_WORD_SIZE; bits++) {
		if (*bits) {
			Word w = *p;
			for (int i = 0; i < sizeof(Word); i++) {
				if (w & 1)
					return (p - bits) * sizeof(Word) + i;
				w >>= 1;
			}
		}
	}

	return numberOfBits;
}

#undef STATEPLEX_BITSET_WORD_SIZE

}

#endif
