/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsondbactor.cpp
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

#include "jsondbactor.h"

namespace Stateplex {

JsonDbActor::~JsonDbActor()
{
	for (ListIterator<Root> iterator(&mRoots); iterator.hasCurrent(); iterator.subsequent()) {
		deleteRoot(iterator.current());
	}
}

JsonDbActor::Root *JsonDbActor::findRoot(const String *name) const
{
	for (Root *r = mRoots.first(); r; r = mRoots.next(r)) {
		if (r->mName->equals(name))
			return r;
	}

	return 0;
}

void JsonDbActor::deleteRoot(Root *root)
{
	root->remove();
	root->mName->destroy(allocator());
	delete root->mObject;
	allocator()->deallocate(root, sizeof(Root));
}

JsonObject *JsonDbActor::createRoot(const String *name)
{
	if (root(name))
		return 0;

	void *memory = allocator()->allocate(sizeof(Root));
	Root *r = new(memory) Root;
	r->mName = String::copy(allocator(), name);
	r->mObject = new JsonObject(0);
	mRoots.addTail(r);

	return r->mObject;
}

bool JsonDbActor::deleteRoot(const String *name)
{
	Root *r = findRoot(name);
	if (!r)
		return false;

	deleteRoot(r);
	return true;
}

}
