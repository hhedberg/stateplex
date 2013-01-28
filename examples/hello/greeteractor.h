/*
 * Stateplex - A server-side actor model library.
 *
 * examples/hello/greeteractor.h
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

#ifndef INCLUDED_GREETER_ACTOR_H
#define INCLUDED_GREETER_ACTOR_H

#include <stateplex/core/actor.h>
#include <stateplex/core/message.h>

struct GreetingMessage {
	Stateplex::Message message; /* Keep first! */
	const char *mText;

	explicit GreetingMessage(const char *text);
	void greet() const;
};

class GreeterActor : public Stateplex::Actor {

	void handleGreet(GreetingMessage *greeting);
public:
	GreeterActor(Stateplex::Dispatcher *dispatcher);
	void greet(GreetingMessage *greeting, Stateplex::Actor *sender);
};

/*** Inline implementations ***/

#include <iostream>

GreetingMessage::GreetingMessage(const char *text)
	: mText(text)
{ }

void GreetingMessage::greet() const
{
	std::cout << mText;
}

GreeterActor::GreeterActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

void GreeterActor::handleGreet(GreetingMessage *greeting)
{
	greeting->greet();
}

void GreeterActor::greet(GreetingMessage *greeting, Stateplex::Actor *sender)
{
	queueMessage(greeting, sender, this, &GreeterActor::handleGreet);
}

#endif
