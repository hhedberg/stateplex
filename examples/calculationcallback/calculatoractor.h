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
	class CalculationMessage : public Stateplex::CallbackMessage {
		int mX;
		int mY;
		int mResult;

	protected:
		void handle(Actor *sender, Actor *receiver);

	public:
		template<typename T> CalculationMessage(Actor *sender, Actor *receiver, int x, int y, T* callbackObject, void (T::*callbackFunction)(Stateplex::CallbackMessage *message));
		int result() const;
	};

	CalculatorActor(Stateplex::Dispatcher *dispatcher);
	template<typename T> void calculateSum(Stateplex::Actor *sender, int x, int y, T* callbackObject, void (T::*callbackFunction)(Stateplex::CallbackMessage *message));
};

/*** Inline implementations ***/

#include <iostream>

template<typename T> CalculatorActor::CalculationMessage::CalculationMessage(Actor *sender, Actor *receiver, int x, int y, T* callbackObject, void (T::*callbackFunction)(Stateplex::CallbackMessage *message))
	: Stateplex::CallbackMessage(sender, receiver, callbackObject, callbackFunction), mX(x), mY(y), mResult(0)
{ }

inline void CalculatorActor::CalculationMessage::handle(Actor *sender, Actor *receiver)
{
	mResult = mX + mY;
	invokeCallback();
}

inline int CalculatorActor::CalculationMessage::result() const
{
	return mResult;
}

inline CalculatorActor::CalculatorActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

template<typename T> void CalculatorActor::calculateSum(Stateplex::Actor *sender, int x, int y, T* callbackObject, void (T::*callbackFunction)(Stateplex::CallbackMessage *message))
{
	CalculationMessage *calculation = new CalculationMessage(sender, this, x, y, callbackObject, callbackFunction);
	queueMessage(calculation);
}

#endif
