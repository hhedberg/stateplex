/*
 * Stateplex - A server-side actor model library.
 *
 * jsondb/jsonobject.h
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

#ifndef INCLUDED_STATEPLEX_JSON_OBJECT_H
#define INCLUDED_STATEPLEX_JSON_OBJECT_H

#include "jsonitem.h"

namespace Stateplex {

class JsonObject : public JsonItem {
	friend class JsonArray;
	friend class JsonDbActor;

	struct Member : public ListItem {
		String *mName;
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

	List<Member> mMembers; // TODO: Use hashtable or similar instead.

	JsonObject(JsonItem *parent);

	Member *member(const String *name) const;
	Member *replaceMember(const String *name);
	void freeMemberValue(Member *m);

public:
	virtual ~JsonObject();

	Type type(const String *name) const;
	bool has(const String *name) const;
	JsonObject *object(const String *name) const;
	JsonArray *array(const String *name) const;
	long int integer(const String *name, long int defaultValue = 0) const;
	double decimal(const String *name, double defaultValue = 0.0) const;
	const String *string(const String *name, const String *defaultValue = 0) const;
	bool boolean(const String *name, bool defaultValue = false) const;
	JsonObject *setObject(const String *name);
	JsonArray *setArray(const String *name);
	void setInteger(const String *name, long int integer);
	void setDecimal(const String *name, double decimal);
	void setString(const String *name, const String *string);
	void setBoolean(const String *name, bool boolean);
	void setNull(const String *name);
	void unset(const String *name);
};

}

/*** Inline implementations ***/

#include "../core/source.h"
#include "jsonarray.h"

namespace Stateplex {

inline JsonObject::JsonObject(JsonItem *parent)
	: JsonItem(parent)
{ }

inline JsonItem::Type JsonObject::type(const String *name) const
{
	Member *m = member(name);
	if (!m)
		return JSON_ITEM_TYPE_INVALID;

	return m->mType;
}

inline bool JsonObject::has(const String *name) const
{
	return !!member(name);
}

inline JsonObject *JsonObject::object(const String *name) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_OBJECT)
		return 0;

	return m->mObject;
}

inline JsonArray *JsonObject::array(const String *name) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_ARRAY)
		return 0;

	return m->mArray;
}

inline long int JsonObject::integer(const String *name, long int defaultValue) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_INTEGER)
		return defaultValue;

	return m->mInteger;
}

inline double JsonObject::decimal(const String *name, double defaultValue) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_DECIMAL)
		return defaultValue;

	return m->mDecimal;
}

inline const String *JsonObject::string(const String *name, const String *defaultValue) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_STRING)
		return defaultValue;

	return m->mString;
}

inline bool JsonObject::boolean(const String *name, bool defaultValue) const
{
	Member *m = member(name);
	if (!m || m->mType != JSON_ITEM_TYPE_BOOLEAN)
		return defaultValue;

	return m->mBoolean;
}

}

#endif
