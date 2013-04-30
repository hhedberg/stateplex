/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsondbactor.h
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

#ifndef INCLUDED_STATEPLEX_JSON_DB_ACTOR_H
#define INCLUDED_STATEPLEX_JSON_DB_ACTOR_H

#include "../core/actor.h"
#include "../core/list.h"

// TODO: Convert the whole thing to use messaging through the dispatcher!

namespace Stateplex {

class JsonObject;
class Dispatcher;
class String;

class JsonDbActor : public Actor {
	struct Root : public ListItem {
		String *mName;
		JsonObject *mObject;
	};

	List<Root> mRoots; // TODO: Use hashtable or similar instead.

	Root *findRoot(const String *name) const;
	void deleteRoot(Root *root);

public:
	JsonDbActor(Dispatcher *dispatcher);
	virtual ~JsonDbActor();

	JsonObject *root(const String *name) const;
	JsonObject *createRoot(const String *name);
	bool deleteRoot(const String *name);
};

}

/*** Inline implementations ***/

#include "../core/string.h"
#include "jsonobject.h"

namespace Stateplex {

inline JsonDbActor::JsonDbActor(Dispatcher *dispatcher)
	: Object(this), Actor(dispatcher)
{ }

inline JsonObject *JsonDbActor::root(const String *name) const
{
	Root *r = findRoot(name);
	if (!r)
		return 0;

	return r->mObject;
}

}

#endif
