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
	Size8 mZero; // Keep first!
	const char *mChars;
	Size mLength;

	String(const char *cString, Size length);

	Size getLength(const char **contents) const;
	void setLength(Size length);

	static Size sizeOfLength(Size length);

public:
	~String();

	const char *chars() const;
	char *chars();
	void destroy(Allocator *allocator);
	Size length() const;
	bool equals(const String *string) const;

	static String *uninitialised(Allocator *allocator, Size length);
	static String *copy(Allocator *allocator, const char *cString);
	static String *copy(Allocator *allocator, const char *cString, Size length);
	static String *copy(Allocator *allocator, const String *string);
	static String reference(const char *cString);
	static String reference(const char *cString, Size length);
	static String reference(const String *string);
};

}

/*** Inline implementations ***/

#include <string.h>

#include "allocator.h"

namespace Stateplex {

inline String::String(const char *cString, Size length)
	: mZero(0), mChars(cString), mLength(length)
{ }

inline String::~String()
{
	if (mZero != 0)
		abort();
}

inline Size String::getLength(const char **contents) const
{
	const Size8 *size = reinterpret_cast<const Size8 *>(this);
	if (*size == 0) {
		*contents = mChars;
		return mLength;
	} else if ((*size & 0x80) == 0) {
		*contents = reinterpret_cast<const char*>(this) + 1;
		return *size;
	} else if ((*(size + 1) & 0x80) == 0) {
		*contents = reinterpret_cast<const char*>(this) + 2;
		return (*size & 0x7f) + (*(size + 1) << 7);
	} else {
		*contents = reinterpret_cast<const char*>(this) + 3;
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
	} else if (length == 0 || length >= 2 << 7) {
		*size = length | 0x80;
		*(size + 1) = length >> 7;
	} else {
		*size = length & 0x7f;
	}
}

inline Size String::sizeOfLength(Size length)
{
	Size size;
	if (length == 0)
		size = 2;
	else if (length >= 2 << 14)
		size = 3;
	else if (length >= 2 << 7)
		size = 2;
	else
		size = 1;

	return size;
}

inline const char *String::chars() const
{
	const char *contents;
	getLength(&contents);
	return contents;
}

inline char *String::chars()
{
	const char *contents;
	getLength(&contents);
	return const_cast<char *>(contents);
}

inline Size String::length() const
{
	const char *contents;
	return getLength(&contents);
}

inline bool String::equals(const String *string) const
{
	const char *mine;
	Size length = getLength(&mine);
	const char *compared;
	Size stringLength = string->getLength(&compared);

	if (length != stringLength)
		return false;

	return memcmp(mine, compared, length) == 0;
}


inline String *String::uninitialised(Allocator *allocator, Size length)
{
	Size size = sizeOfLength(length);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length + 1));
	*(memory + size + length) = 0;
	String *string = reinterpret_cast<String *>(memory);
	string->setLength(length);

	return string;
}

inline String *String::copy(Allocator *allocator, const char *cString)
{
	return copy(allocator, cString, strlen(cString));
}

inline String *String::copy(Allocator *allocator, const char *cString, Size length)
{
	Size size = sizeOfLength(length);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length + 1));
	memcpy(memory + size, cString, length);
	*(memory + size + length) = 0;
	String *string = reinterpret_cast<String *>(memory);
	string->setLength(length);

	return string;
}

inline String *String::copy(Allocator *allocator, const String *string)
{
	const char *contents;
	Size length = string->getLength(&contents);
	Size size = sizeOfLength(length);
	char *memory = reinterpret_cast<char *>(allocator->allocate(size + length + 1));
	memcpy(memory + size, reinterpret_cast<const char *>(string) + size, length + 1);
	String *newString = reinterpret_cast<String *>(memory);
	newString->setLength(length);

	return newString;
}

inline void String::destroy(Allocator *allocator)
{
	if (mZero == 0)
		abort();

	const char *contents;
	Size length = getLength(&contents);
	Size size = sizeOfLength(length);
	size += length + 1;
	allocator->deallocate(reinterpret_cast<void *>(this), size);
}

inline String String::reference(const char *cString)
{
	return String(cString, strlen(cString));
}

inline String String::reference(const char *cString, Size length)
{
	return String(cString, length);
}

inline String String::reference(const String *string)
{
	return String(string->chars(), string->length());
}

}

#endif
