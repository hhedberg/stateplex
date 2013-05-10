/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsonobject.cpp
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

#include "jsonobject.h"
#include "../core/receiver.h"
#include "../core/writebuffer.h"

namespace Stateplex {

JsonObject::~JsonObject()
{
	Member *n;
	for (Member *m = mMembers.first(); m; m = n) {
		n = mMembers.next(m);
		freeMemberValue(m);
		m->mName->destroy(allocator());
		delete m;
	}
}

JsonObject::Member *JsonObject::member(const String *name) const
{
	for (Member *m = mMembers.first(); m; m = mMembers.next(m)) {
		if (m->mName->equals(name))
			return m;
	}

	return 0;
}

JsonObject::Member *JsonObject::replaceMember(const String *name)
{
	Member *m = member(name);
	if (m) {
		freeMemberValue(m);
	} else {
		m = new Member();
		m->mName = String::copy(allocator(), name);
		m->mEscaped = 0;
		mMembers.addTail(m);
	}

	return m;
}

void JsonObject::freeMemberValue(Member *m)
{
	if (m->mType == JSON_ITEM_TYPE_ARRAY)
		delete m->mArray;
	else if (m->mType == JSON_ITEM_TYPE_OBJECT)
		delete m->mObject;
	else if (m->mType == JSON_ITEM_TYPE_STRING)
		m->mString->destroy(allocator());
	if (m->mEscaped) {
		m->mEscaped->destroy(allocator());
		m->mEscaped = 0;
	}
}

void JsonObject::escapeMember(Member *m) const
{
	WriteBuffer buffer(actor());
	String *name = escape(m->mName);
	buffer.append(name);
	name->destroy(allocator());
	buffer.append(":", 1);
	if (m->mType == JSON_ITEM_TYPE_BOOLEAN) {
		if(m->mBoolean) {
			buffer.append("true", 4);
		} else {
			buffer.append("false", 5);
		}
	} else if (m->mType == JSON_ITEM_TYPE_DECIMAL) {
		String *decimal = escape(m->mDecimal);
		buffer.append(decimal);
		decimal->destroy(allocator());
	} else if (m->mType == JSON_ITEM_TYPE_INTEGER) {
		String *integer = escape(m->mInteger);
		buffer.append(integer);
		integer->destroy(allocator());
	} else if (m->mType == JSON_ITEM_TYPE_NULL) {
		buffer.append("null", 4);
	} else if (m->mType == JSON_ITEM_TYPE_STRING) {
		String *string = escape(m->mString);
		buffer.append(string);
		string->destroy(allocator());
	}
	m->mEscaped = buffer.asString();
}

const String *JsonObject::asString(const String *name) const
{
	Member *m = member(name);
	if (!m)
		return 0;

	if (!m->mEscaped)
		escapeMember(m);
	return m->mEscaped;
}

const String *JsonObject::name(JsonItem *item) const
{
	for (Member *m = mMembers.first(); m; m = mMembers.next(m)) {
		if (m->mObject == item)
			return m->mName;
	}

	return 0;
}

JsonObject *JsonObject::setObject(const String *name)
{
	JsonObject *object = new JsonObject(this);
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_OBJECT;
	m->mObject = object;

	notifyMemberSet(this, name);

	return object;
}

JsonArray *JsonObject::setArray(const String *name)
{
	JsonArray *array = new JsonArray(this);
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_ARRAY;
	m->mArray = array;

	notifyMemberSet(this, name);

	return array;
}

void JsonObject::setInteger(const String *name, long int integer)
{
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_INTEGER;
	m->mInteger = integer;

	notifyMemberSet(this, name);
}

void JsonObject::setDecimal(const String *name, double decimal)
{
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_DECIMAL;
	m->mDecimal = decimal;

	notifyMemberSet(this, name);
}

void JsonObject::setString(const String *name, const String *string)
{
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_STRING;
	m->mString = String::copy(Dispatcher::current()->allocator(), string);

	notifyMemberSet(this, name);
}

void JsonObject::setBoolean(const String *name, bool boolean)
{
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_BOOLEAN;
	m->mBoolean = boolean;

	notifyMemberSet(this, name);
}

void JsonObject::setNull(const String *name)
{
	Member *m = replaceMember(name);
	m->mType = JSON_ITEM_TYPE_NULL;

	notifyMemberSet(this, name);
}

JsonItem::Type JsonObject::type() const
{
	return JSON_ITEM_TYPE_OBJECT;
}

void JsonObject::unset(const String *name)
{
	Member *m = member(name);
	if (!m)
		return;

	freeMemberValue(m);
	m->mName->destroy(Dispatcher::current()->allocator());
	delete m;

	notifyMemberUnset(this, name);
}

void JsonObject::send(Receiver *receiver, Size depth) const
{
	receiver->receive("{", 1);
	if (depth > 0) {
		bool first = true;
		for (Member *m = mMembers.first(); m; m = mMembers.next(m)) {
			if (first) {
				first = false;
			} else {
				receiver->receive(",", 1);
			}
			if (!m->mEscaped)
				escapeMember(m);
			receiver->receive(m->mEscaped);

			if (m->mType == JSON_ITEM_TYPE_ARRAY) {
				m->mArray->send(receiver, depth - 1);
			} else if (m->mType == JSON_ITEM_TYPE_OBJECT) {
				m->mObject->send(receiver, depth - 1);
			}
		}
	}
	receiver->receive("}", 1);
}

}
