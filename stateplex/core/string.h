/*
 * Stateplex - A server-side actor model library.
 *
 * core/string.h
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

#ifndef INCLUDED_STATEPLEX_STRING_H
#define INCLUDED_STATEPLEX_STRING_H

#include "types.h"

namespace Stateplex {

class Allocator;

class String {
	String();
	~String();

	Size getLength(Size *sizeOfLength) const;
	void setLength(Size length);

	static Size sizeOfLength(Size length);

public:
	const char *chars() const;
	char *chars();
	void destroy(Allocator *allocator);
	Size length() const;

	static String *uninitialised(Allocator *allocator, Size length);
	static String *copy(Allocator *allocator, const char *cString);
	static String *copy(Allocator *allocator, const char *cString, Size length);
};

}

/*** Inline implementations ***/

#include <string.h>

#include "allocator.h"

namespace Stateplex {

inline Size String::getLength(Size *sizeOfLength) const
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

inline void String::setLength(Size length)
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

inline Size String::sizeOfLength(Size length)
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

inline const char *String::chars() const
{
	Size size;
	getLength(&size);
	return reinterpret_cast<const char *>(this) + size;
}

inline char *String::chars()
{
	Size size;
	getLength(&size);
	return reinterpret_cast<char *>(this) + size;
}

inline Size String::length() const
{
	Size size;
	return getLength(&size);
}

inline String *String::uninitialised(Allocator *allocator, Size length)
{
	Size size;
	char *string = reinterpret_cast<char *>(allocator->allocate(size + length + 1));
	*(string + size + length) = 0;

	return reinterpret_cast<String *>(string);
}

inline String *String::copy(Allocator *allocator, const char *cString)
{
	return copy(allocator, cString, strlen(cString));
}

inline String *String::copy(Allocator *allocator, const char *cString, Size length)
{
	Size size = sizeOfLength(length);;
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length + 1));
	memcpy(memory + size, cString, length);
	*(memory + size + length) = 0;
	String *string = reinterpret_cast<String *>(memory);
	string->setLength(length);

	return string;
}

inline void String::destroy(Allocator *allocator)
{
	Size size;
	Size length = getLength(&size);
	size += length + 1;
	allocator->deallocate(reinterpret_cast<void *>(this), size);
}

}

#endif
