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

#ifndef INCLUDED_CALCULATOR_ACTOR_H
#define INCLUDED_CALCULATOR_ACTOR_H

#include <stateplex/core/actor.h>
#include <stateplex/core/callbackmessage.h>

class CalculatorActor : public Stateplex::Actor {
public:
	class CalculationMessage : public Stateplex::CallbackMessage<CalculatorActor, CalculationMessage> {
		int mX;
		int mY;
		int mResult;

	protected:
		void handle(Actor *sender, CalculatorActor *receiver);

	public:
		template<typename Sender> CalculationMessage(Sender *sender, CalculatorActor *receiver, int x, int y, void (Sender::*callbackFunction)(CalculationMessage *message));
		int result() const;
	};

	CalculatorActor(Stateplex::Dispatcher *dispatcher);
	template<typename Sender> void calculateSum(Sender *sender, int x, int y, void (Sender::*callbackFunction)(CalculationMessage *message));
};

/*** Inline implementations ***/

#include <iostream>

template<typename Sender> CalculatorActor::CalculationMessage::CalculationMessage(Sender *sender, CalculatorActor *receiver, int x, int y, void (Sender::*callbackFunction)(CalculationMessage *message))
	: Stateplex::CallbackMessage<CalculatorActor, CalculationMessage>(sender, receiver, callbackFunction), mX(x), mY(y), mResult(0)
{ }

inline void CalculatorActor::CalculationMessage::handle(Actor *sender, CalculatorActor *receiver)
{
	mResult = mX + mY;
	invokeCallback(this);
}

inline int CalculatorActor::CalculationMessage::result() const
{
	return mResult;
}

inline CalculatorActor::CalculatorActor(Stateplex::Dispatcher *dispatcher)
	: Object(this), Actor(dispatcher)
{ }

template<typename Sender> void CalculatorActor::calculateSum(Sender *sender, int x, int y, void (Sender::*callbackFunction)(CalculationMessage *message))
{
	CalculationMessage *calculation = new CalculationMessage(sender, this, x, y, callbackFunction);
	queueMessage(calculation);
}

#endif
