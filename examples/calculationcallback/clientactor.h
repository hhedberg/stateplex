/*
 * Stateplex - A server-side actor model library.
 *
 * examples/hello/calculatoractor.h
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

#ifndef INCLUDED_CLIENT_ACTOR_H
#define INCLUDED_CLIENT_ACTOR_H

#include <stateplex/core/actor.h>
#include <stateplex/core/callbackmessage.h>

#include "calculatoractor.h"

class ClientActor : public Stateplex::Actor {

public:
	ClientActor(Stateplex::Dispatcher *dispatcher);

	void showResult(CalculatorActor::CalculationMessage *message);
};

/*** Inline implementations ***/

#include <iostream>

ClientActor::ClientActor(Stateplex::Dispatcher *dispatcher)
	: Object(this), Actor(dispatcher)
{ }

void ClientActor::showResult(CalculatorActor::CalculationMessage *message)
{
	std::cout << "Result is " << message->result() << std::endl;
}

#endif
