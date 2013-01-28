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

template<Size numberOfBits>
inline Bitset<numberOfBits>::Bitset(bool setBits)
{
	memset(bits, setBits ? 1 : 0, STATEPLEX_BITSET_WORD_SIZE * sizeof(Word));
}

template<Size numberOfBits>
inline Size Bitset<numberOfBits>::size()
{
	return STATEPLEX_BITSET_WORD_SIZE * sizeof(Word);
}

template<Size numberOfBits>
inline void Bitset<numberOfBits>::setBit(Size bit)
{
	bits[bit / sizeof(Word)] |= 1 << (bit % sizeof(Word));
}
	
template <Size numberOfBits>
void Bitset<numberOfBits>::unsetBit(Size bit)
{
	bits[bit / sizeof(Word)] &= ~(1 << (bit % sizeof(Word)));
}

template <Size numberOfBits>
bool Bitset<numberOfBits>::isBitSet(Size bit)
{
	return bits[bit / sizeof(Word)] & (1 << (bit % sizeof(Word)));
}

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
