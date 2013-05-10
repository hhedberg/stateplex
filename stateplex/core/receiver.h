/*
 * Stateplex - A server-side actor model library.
 *
 * core/receiver.h
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

#ifndef INCLUDED_STATEPLEX_RECEIVER_H
#define INCLUDED_STATEPLEX_RECEIVER_H

#include "types.h"
#include "buffer.h"

namespace Stateplex {

class String;

class Receiver {
public:
	virtual ~Receiver() = 0;

	virtual void receiveEnd() = 0;
	virtual bool receive(const String *string) = 0;
	virtual bool receive(Buffer *buffer) = 0;

	bool receive(const char *data, Size length);
	bool receive(const char *cString);
};

}

/*** Inline implementations ***/

#include "string.h"

namespace Stateplex {

inline Receiver::~Receiver()
{ }

inline bool Receiver::receive(const char *data, Size length)
{
	String string = String::reference(data, length);
	return receive(&string);
}

inline bool Receiver::receive(const char *cString)
{
	String string = String::reference(cString);
	return receive(&string);
}

}

#endif
