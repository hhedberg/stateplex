/*
 * Stateplex - A server-side actor model library.
 *
 * core/method.h
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

#ifndef INCLUDED_STATEPLEX_METHOD_H
#define INCLUDED_STATEPLEX_METHOD_H

class Unknown;

namespace Stateplex {

class Method {
	Unknown *mObject;
	void (Unknown::*mFunction)(Unknown *argument);

	void warnUninitialisedMethod(Unknown *argument);
public:
	Method();
	template<typename Type, typename Argument> Method(Type *object, void (Type::*function)(Argument *argument));
	template<typename Type, typename Argument> void set(Type *object, void (Type::*function)(Argument *argument));
	template<typename Argument> void invoke(Argument *argument) const;
};

}

/*** Template implementations ***/

namespace Stateplex {

inline Method::Method()
	: mObject(reinterpret_cast<Unknown *>(this)), mFunction(reinterpret_cast<void (Unknown::*)(Unknown *)>(&Method::warnUninitialisedMethod))
{ }

template<typename Type, typename Argument>
inline Method::Method(Type *object, void (Type::*function)(Argument *argument))
	: mObject(reinterpret_cast<Unknown *>(object)), mFunction(reinterpret_cast<void (Unknown::*)(Unknown *)>(function))
{ }

template<typename Type, typename Argument>
inline void Method::set(Type *object, void (Type::*function)(Argument *argument))
{
	mObject = reinterpret_cast<Unknown *>(object);
	mFunction = reinterpret_cast<void (Unknown::*)(Unknown *)>(function);
}

template<typename Argument>
inline void Method::invoke(Argument *argument) const
{
	(mObject->*mFunction)(reinterpret_cast<Unknown *>(argument));
}

}

#endif
