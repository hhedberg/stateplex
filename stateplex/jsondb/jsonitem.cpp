/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsonitem.cpp
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

#include <stdio.h>

#include "jsonitem.h"
#include "jsonobject.h"
#include "../core/writebuffer.h"

namespace Stateplex {

JsonItem::Observer::~Observer()
{ }

JsonItem::~JsonItem()
{
	for (Observer *o = mObservers.first(); o; o = mObservers.next(o)) {
		o->itemDeleted(this);
	}
}

void JsonItem::notifyElementInserted(JsonArray *array, int index) const
{
	const JsonItem *item = this;
	do {
		for (Observer *o = item->mObservers.first(); o; o = item->mObservers.next(o)) {
			o->elementInserted(array, index);
		}
		item = item->mParent;
	} while (item);
}

void JsonItem::notifyElementRemoved(JsonArray *array, int index) const
{
	const JsonItem *item = this;
	do {
		for (Observer *o = item->mObservers.first(); o; o = item->mObservers.next(o)) {
			o->elementRemoved(array, index);
		}
		item = item->mParent;
	} while (item);
}

void JsonItem::notifyMemberSet(JsonObject *object, const String *name) const
{
	const JsonItem *item = this;
	do {
		for (Observer *o = item->mObservers.first(); o; o = item->mObservers.next(o)) {
			o->memberSet(object, name);
		}
		item = item->mParent;
	} while (item);

}

void JsonItem::notifyMemberUnset(JsonObject *object, const String *name) const
{
	const JsonItem *item = this;
	do {
		for (Observer *o = item->mObservers.first(); o; o = item->mObservers.next(o)) {
			o->memberUnset(object, name);
		}
		item = item->mParent;
	} while (item);

}

String *JsonItem::escape(long int integer) const
{
	char buffer[128];
	Size length = snprintf(buffer, sizeof(buffer), "%ld", integer);
	return String::copy(allocator(), buffer, length);
}

String *JsonItem::escape(double decimal) const
{
	char buffer[128];
	Size length = snprintf(buffer, sizeof(buffer), "%lf", decimal);
	return String::copy(allocator(), buffer, length);
}

String *JsonItem::escape(const String *string) const
{
	WriteBuffer<> buffer(actor());
	buffer.append("\"", 1);
	const char *chars = string->chars();
	for (Size i = 0; i < string->length(); i++) {
		switch (chars[i]) {
		case '"':
			buffer.append("\\\"", 2);
			break;
		default:
			buffer.append(&chars[i], 1);
			break;
		}
	}
	buffer.append("\"", 1);

	return buffer.asString();
}

}
