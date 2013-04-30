/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsonarray.cpp
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

#include "jsonarray.h"

namespace Stateplex {

JsonArray::~JsonArray()
{
	Member *n;
	for (Member *m = mMembers.first(); m; m = n) {
		n = mMembers.next(m);
		freeMemberValue(m);
		delete m;
	}
}

JsonArray::Member *JsonArray::member(Size index) const
{
	Member *m = mMembers.first();
	for (Size i = 0; i < index; i++)
		m = mMembers.next(m);

	return m;
}

void JsonArray::freeMemberValue(Member *m)
{
	if (m->mType == JSON_ITEM_TYPE_ARRAY)
		delete m->mArray;
	else if (m->mType == JSON_ITEM_TYPE_OBJECT)
		delete m->mObject;
	else if (m->mType == JSON_ITEM_TYPE_STRING)
		m->mString->destroy(Dispatcher::current()->allocator());
}

JsonObject *JsonArray::appendObject()
{
	JsonObject *object = new JsonObject(this);
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_OBJECT;
	m->mObject = object;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);

	return object;
}

JsonArray *JsonArray::appendArray()
{
	JsonArray *array = new JsonArray(this);
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_ARRAY;
	m->mArray = array;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);

	return array;

}

void JsonArray::appendInteger(long int integer)
{
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_INTEGER;
	m->mInteger = integer;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);
}

void JsonArray::appendDecimal(double decimal)
{
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_DECIMAL;
	m->mDecimal = decimal;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);
}

void JsonArray::appendString(const String *string)
{
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_STRING;
	m->mString = String::copy(Dispatcher::current()->allocator(), string);
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);
}

void JsonArray::appendBoolean(bool boolean)
{
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_BOOLEAN;
	m->mBoolean = boolean;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);
}

void JsonArray::appendNull()
{
	Member *m = new Member();
	m->mType = JSON_ITEM_TYPE_NULL;
	mMembers.addTail(m);
	mLength++;

	notifyElementInserted(this, mLength);
}

void JsonArray::remove(Size index)
{
	if (index >= mLength)
		return;

	Member *m = member(index);
	m->remove();
	freeMemberValue(m);
	delete m;
	mLength--;

	notifyElementRemoved(this, index);
}

}
