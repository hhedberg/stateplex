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

#ifndef INCLUDED_STATEPLEX_FILTER_H
#define INCLUDED_STATEPLEX_FILTER_H

#include "receiver.h"

namespace Stateplex {

class String;

class Filter : public Receiver {
	Receiver *mReceiver;

public:
	Filter(Receiver *receiver = 0);

	Receiver *receiver() const;
	void setReceiver(Receiver *receiver);
};

}

/*** Inline implementations ***/

#include "string.h"

namespace Stateplex {

Filter::Filter(Receiver *receiver)
	: mReceiver(receiver)
{ }

inline Receiver *Filter::receiver() const
{
	return mReceiver;
}

inline void Filter::setReceiver(Receiver *receiver)
{
	mReceiver = receiver;
}

}

#endif
