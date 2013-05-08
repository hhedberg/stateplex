/*
 * Stateplex - A server-side actor model library.
 *
 * core/identityfilter.cpp
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

#include "identityfilter.h"

namespace Stateplex {

void IdentityFilter::receiveEnd()
{
	receiver()->receiveEnd();
}

void IdentityFilter::receive(const String *string)
{
	receiver()->receive(string);
}

void IdentityFilter::receive(Buffer *buffer)
{
	receiver()->receive(buffer);
}

}
