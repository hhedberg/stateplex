/*
 * Stateplex - A server-side actor model library.
 *
 * core/object.h
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

#ifndef INCLUDED_STATEPLEX_OBJECT_H
#define INCLUDED_STATEPLEX_OBJECT_H

#include <new>
#include <stdlib.h>

namespace Stateplex {

class Actor;
class Allocator;

class Object {
	Actor *mActor;

public:
	Object(Actor *actor);
	virtual ~Object();

	Actor *actor() const;
	Allocator *allocator() const;

	static void *operator new(size_t size);
	static void operator delete(void *memory, size_t size);
};

}

/*** Inline implementations ***/

#include "dispatcher.h"
#include "allocator.h"

namespace Stateplex {

inline Object::Object(Actor *actor)
	: mActor(actor)
{ }

inline Object::~Object()
{ }

inline Actor *Object::actor() const
{
	return mActor;
}

inline Allocator *Object::allocator() const
{
	return Dispatcher::current()->allocator();
}


inline void *Object::operator new(size_t size)
{
	return Dispatcher::current()->allocator()->allocate(size);
}

inline void Object::operator delete(void *memory, size_t size)
{
	Dispatcher::current()->allocator()->deallocate(memory, size);
}

}

#endif
