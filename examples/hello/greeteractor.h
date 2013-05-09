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

class GreeterActor : public Stateplex::Actor {
	class GreetingMessage : public Stateplex::Message<GreeterActor> {
		const char *mText;

	protected:
		void handle(Actor *sender, GreeterActor *receiver);

	public:
		GreetingMessage(Actor *sender, GreeterActor *receiver, const char *text);
	};

public:
	GreeterActor(Stateplex::Dispatcher *dispatcher);
	void greet(Stateplex::Actor *sender, const char *text);
};

/*** Inline implementations ***/

#include <iostream>

GreeterActor::GreetingMessage::GreetingMessage(Actor *sender, GreeterActor *receiver, const char *text)
	: Stateplex::Message<GreeterActor>(sender, receiver), mText(text)
{ }

void GreeterActor::GreetingMessage::handle(Actor *sender, GreeterActor *receiver)
{
	std::cout << mText;
}

GreeterActor::GreeterActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

void GreeterActor::greet(Stateplex::Actor *sender, const char *text)
{
	GreetingMessage *greeting = new GreetingMessage(sender, this, text);
	queueMessage(greeting);
}

#endif
