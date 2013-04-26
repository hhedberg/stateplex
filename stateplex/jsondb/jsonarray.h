/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsonarray.h
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

#ifndef INCLUDED_STATEPLEX_JSON_ARRAY_H
#define INCLUDED_STATEPLEX_JSON_ARRAY_H

#include "jsonitem.h"

namespace Stateplex {

class JsonArray : public JsonItem {
	struct Member : public ListItem {
		Type mType;
		union {
			JsonObject *mObject;
			JsonArray *mArray;
			String *mString;
			long int mInteger;
			double mDecimal;
			bool mBoolean;
		};
	};

	List<Member> mMembers;
	Size mLength;

	Member *member(Size index) const;
	void freeMemberValue(Member *m);

public:
	JsonArray(JsonItem *parent);
	~JsonArray();

	Size length() const;
	Type type(Size index) const;
	JsonObject *object(Size index) const;
	JsonArray *array(Size index) const;
	long int integer(Size index, long int defaultValue = 0) const;
	double decimal(Size index, double defaultValue = 0.0) const;
	const String *string(Size index, const String *defaultValue = 0) const;
	bool boolean(Size index, bool defaultValue = false) const;
	JsonObject *appendObject();
	JsonArray *appendArray();
	void appendInteger(long int integer);
	void appendDecimal(double decimal);
	void appendString(const String *string);
	void appendBoolean(bool boolean);
	void appendNull();
	void remove(Size index);
};

}

/*** Inline implementations ***/

#include "../core/source.h"
#include "jsonobject.h"

namespace Stateplex {

inline JsonArray::JsonArray(JsonItem *parent)
	: JsonItem(parent), mLength(0)
{ }

inline void JsonArray::freeMemberValue(Member *m)
{
	if (m->mType == JSON_ITEM_TYPE_ARRAY)
		delete m->mArray;
	else if (m->mType == JSON_ITEM_TYPE_OBJECT)
		delete m->mObject;
	else if (m->mType == JSON_ITEM_TYPE_STRING)
		m->mString->destroy(Dispatcher::current()->allocator());
}

inline Size JsonArray::length() const
{
	return mLength;
}

inline JsonItem::Type JsonArray::type(Size index) const
{
	if (index >= mLength)
		return JSON_ITEM_TYPE_INVALID;

	return member(index)->mType;
}

inline JsonObject *JsonArray::object(Size index) const
{
	if (index >= mLength)
		return 0;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_OBJECT)
		return 0;

	return m->mObject;
}

inline JsonArray *JsonArray::array(Size index) const
{
	if (index >= mLength)
		return 0;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_ARRAY)
		return 0;

	return m->mArray;
}

inline long int JsonArray::integer(Size index, long int defaultValue) const
{
	if (index >= mLength)
		return defaultValue;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_INTEGER)
		return defaultValue;

	return m->mInteger;
}

inline double JsonArray::decimal(Size index, double defaultValue) const
{
	if (index >= mLength)
		return defaultValue;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_DECIMAL)
		return defaultValue;

	return m->mDecimal;
}

inline const String *JsonArray::string(Size index, const String *defaultValue) const
{
	if (index >= mLength)
		return defaultValue;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_STRING)
		return defaultValue;

	return m->mString;
}

inline bool JsonArray::boolean(Size index, bool defaultValue) const
{
	if (index >= mLength)
		return defaultValue;

	Member *m = member(index);
	if (m->mType != JSON_ITEM_TYPE_BOOLEAN)
		return defaultValue;

	return m->mBoolean;
}

}

#endif
