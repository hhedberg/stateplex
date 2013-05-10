/*
 * Stateplex - A server-side actor model library.
 *
 * core/array.h
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

#ifndef INCLUDED_STATEPLEX_ARRAY_H
#define INCLUDED_STATEPLEX_ARRAY_H

#include "types.h"

namespace Stateplex {

class Allocator;

template<typename Type>
class Array {
	Array();
	~Array();

	Size getLength(Size *sizeOfLength) const;
	void setLength(Size length);

	static Size sizeOfLength(Size length);

public:
	Type *elements() const;
	Type element(Size index) const;
	Size length() const;

	void setElement(Size index, Type value);
	void destroy(Allocator *allocator);

	static Array<Type> *uninitialised(Allocator *allocator, Size length);
	static Array<Type> *copy(Allocator *allocator, Type *const cArray, Size length);
	static Array<Type> *copy(Allocator *allocator, Array<Type> *array);
};

}

/*** Inline implementations ***/

#include <string.h>
#include <stdlib.h>

#include "allocator.h"

namespace Stateplex {

/*
 * Checks and gets the length of the array.
 */

template<typename Type>
inline Size Array<Type>::getLength(Size *sizeOfLength) const
{
	const Size8 *size = reinterpret_cast<const Size8 *>(this);
	if ((*size & 0x80) == 0) {
		*sizeOfLength = 1;
		return *size;
	} else if ((*(size + 1) & 0x80) == 0) {
		*sizeOfLength = 2;
		return (*size & 0x7f) + (*(size + 1) << 7);
	} else {
		*sizeOfLength = 3;
		return (*size & 0x7f) + ((*(size + 1) & 0x7f) << 7) + ((*(size + 2) & 0x7f) << 14);
	}
}

/*
 * Checks and sets the array length.
 */

template<typename Type>
inline void Array<Type>::setLength(Size length)
{
	Size8 *size = reinterpret_cast<Size8 *>(this);
	if (length >= 2 << 14) {
		*size = length | 0x80;
		*(size + 1) = length >> 7 | 0x80;
		*(size + 2) = length >> 14;
	} else if (length >= 2 << 7) {
		*size = length | 0x80;
		*(size + 1) = length >> 7;
	} else {
		*size = length & 0x7f;
	}
}

/*
 * Checks the size of the length and return the size.
 */

template<typename Type>
inline Size Array<Type>::sizeOfLength(Size length)
{
	Size size;

	if (length >= 2 << 14)
		size = 3;
	else if (length >= 2 << 7)
		size = 2;
	else
		size = 1;

	return size;
}

/*
 * Return a Type of size long.
 */

template<typename Type>
Type *Array<Type>::elements() const
{
	Size size;
	getLength(&size);

	Array<Type> *self = const_cast<Array<Type> *>(this);
	return *reinterpret_cast<Type **>(reinterpret_cast<char *>(self) + size);
}

/*
 * Return a Type of size long and index.
 */

template<typename Type>
Type Array<Type>::element(Size index) const
{
	Size size;
	if (index >= getLength(&size))
		abort();

	Array<Type> *self = const_cast<Array<Type> *>(this);
	return *(reinterpret_cast<Type *>(reinterpret_cast<char *>(self) + size) + index);
}

/*
 * Takes index and value and sets element.
 */

template<typename Type>
void Array<Type>::setElement(Size index, Type value)
{
	Size size;
	if (index >= getLength(&size))
		abort();

	*(reinterpret_cast<Type *>(reinterpret_cast<char *>(this) + size) + index) = value;
}

/*
 * Returns the length of the array.
 */

template<typename Type>
inline Size Array<Type>::length() const
{
	Size size;
	return getLength(&size);
}

/*
 * Allocates memory and sets the length of uninitialised
 * array and return array.
 */

template<typename Type>
inline Array<Type> *Array<Type>::uninitialised(Allocator *allocator, Size length)
{
	Size size = sizeOfLength(length);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length * sizeof(Type *)));
	Array *array = reinterpret_cast<Array *>(memory);
	array->setLength(length);

	return array;
}

/*
 * Copies and set the length of the array
 *
 * @return 	allocated array length.
 */

template<typename Type>
inline Array<Type> *Array<Type>::copy(Allocator *allocator, Type *const cArray, Size length)
{
	Size size = sizeOfLength(length);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length * sizeof(Type *)));
	memcpy(memory + size, cArray, length * sizeof(Type *));
	Array *array = reinterpret_cast<Array *>(memory);
	array->setLength(length);

	return array;
}

/*
 * Copies the array and allocate memory.
 *
 * @return	allocated array memory.
 */

template<typename Type>
inline Array<Type> *Array<Type>::copy(Allocator *allocator, Array<Type> *array)
{
	Size size;
	Size length = array->getLength(&size);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length * sizeof(Type *)));
	memcpy(memory, array, size + length * sizeof(Type *));

	return reinterpret_cast<Array *>(memory);
}

/*
 * Destroys the allocated array.
 */

template<typename Type>
inline void Array<Type>::destroy(Allocator *allocator)
{
	Size size;
	Size length = getLength(&size);
	size += length * sizeof(Type *);
	allocator->deallocate(reinterpret_cast<void *>(this), size);
}

}

#endif
