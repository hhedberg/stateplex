/*
 * Stateplex - A server-side actor model library.
 *
 * core/factorymethod.h
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

#ifndef INCLUDED_STATEPLEX_FACTORY_METHOD_H
#define INCLUDED_STATEPLEX_FACTORY_METHOD_H

class Unknown;

namespace Stateplex {

/** 
 * @brief Works as a pointer to a member function.
 */
 
template<typename Return, typename Argument>
class FactoryMethod {
	Unknown *mObject;
	Return *(Unknown::*mFunction)(const Argument *argument);

	Return *warnUninitialisedMethod(const Argument *argument);

public:
	FactoryMethod();
	template<typename Type> FactoryMethod(Type *object, Return *(Type::*function)(const Argument *argument));

	template<typename Type> void set(Type *object, Return *(Type::*function)(const Argument *argument));
	Return *invoke(const Argument *argument) const;
};

}

/*** Template implementations ***/

#include "method.h"

namespace Stateplex {

/** 
 * Constructor for FactoryMethod class.
 * Initializes a new instance of class FactoryMethod.
 * 
 */

template<typename Return, typename Argument>
FactoryMethod<Return, Argument>::FactoryMethod()
	: mObject(reinterpret_cast<Unknown *>(this)), mFunction(reinterpret_cast<Return *(Unknown::*)(const Argument *)>(&Method::warnUninitialisedMethod))
{ }

/** 
 * Constructor for FactoryMethod class.
 * Initializes a new instance of class FactoryMethod.
 * Values for handler object and handler function can be given with arguments.
 *
 * @param *object		pointer to handler object.
 * @param *function		pointer to handler function.
 */

template<typename Return, typename Argument> template<typename Type>
FactoryMethod<Return, Argument>::FactoryMethod(Type *object, Return *(Type::*function)(const Argument *argument))
	: mObject(reinterpret_cast<Unknown *>(object)), mFunction(reinterpret_cast<Return *(Unknown::*)(Unknown *)>(function))
{ }

/**
 * Function that can be used to set the handler object and handler function.
 *
 * @param *object		pointer to handler object.
 * @param *function		pointer to handler function.
 */

template<typename Return, typename Argument> template<typename Type>
inline void FactoryMethod<Return, Argument>::set(Type *object, Return *(Type::*function)(const Argument *argument))
{
	mObject = reinterpret_cast<Unknown *>(object);
	mFunction = reinterpret_cast<Return *(Unknown::*)(const Argument *)>(function);
}

/**
 * Function that invokes handler object.
 *
 * @param *argument		pointer to the object that the handler needs. 
 * @return			returns what handler object returns.
 */

template<typename Return, typename Argument>
inline Return *FactoryMethod<Return, Argument>::invoke(const Argument *argument) const
{
	return (mObject->*mFunction)(argument);
}

}

#endif
