/*
 * Stateplex - A server-side actor model library.
 *
 * core/terminalreceiver.h
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

#ifndef INCLUDED_STATEPLEX_TERMINAL_RECEIVER_H
#define INCLUDED_STATEPLEX_TERMINAL_RECEIVER_H

#include "receiver.h"

namespace Stateplex {

class TerminalReceiver : public Object, public Receiver {
public:
	TerminalReceiver(Actor *actor);
	virtual ~TerminalReceiver();

	virtual void receiveEnd();
	virtual void receive(const String *string);
	virtual void receive(Buffer<> *buffer);
};

}

/*** Inline implementations ***/

namespace Stateplex {

TerminalReceiver::TerminalReceiver(Actor *actor)
	: Object(actor)
{ }

TerminalReceiver::~TerminalReceiver()
{ }

}

#endif
