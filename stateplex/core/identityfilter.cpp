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

/*
 * Sets receiver to say it is EOF.
 */

void IdentityFilter::receiveEnd()
{
	receiver()->receiveEnd();
}

/*
 * Receives string from identifyfilters receiver.
 */

bool IdentityFilter::receive(const String *string)
{
	return receiver()->receive(string);
}

/*
 * Receives pointer to a buffer from indentifyfilters
 * receiver.
 */

bool IdentityFilter::receive(Buffer *buffer)
{
	return receiver()->receive(buffer);
}

}
