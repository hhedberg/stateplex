#include "jsonbool.h"
#include <iostream>

/**
 * Constructor that assigns the key, value and owner
 * of the jsonbool object.
 *
 * @param *owner	pointer to the owner actor.
 * @param *key		wanted value for the key.
 * @param b		wanted boolean value.
 */

JsonBool::JsonBool(Stateplex::Actor *owner, const char *key, bool b)
	: JsonItem(owner)
{
	mKey = const_cast<char*>(key);
	mValue = b;
	mType = JSON_BOOL;
}

/**
 * Prints out the key and value of the jsonbool object.
 * 
 * @see JsonObject::traverse()
 */

void JsonBool::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

/**
 * Returns the value of the jsonbool object.
 *
 * @return		value of jsonbool object.
 */

bool JsonBool::value()
{
	return mValue;
}

/**
 * Returns the key of the jsonbool object.
 *
 * @return		key of jsonbool object.
 */

const char *JsonBool::key()
{
	return const_cast<const char*>(mKey);
}

/**
 * Returns the type of the jsonbool object.
 * Can be used for comparison to verify that
 * an object is actually of jsonbool type.
 *
 * @return		type of the jsonbool object.
 */

JsonType JsonBool::type()
{
	return mType;
}

/**
 * Takes given boolean value and assigns it 
 * to jsonbool objects value.
 *
 * @param value		boolean value to set.
 */

void JsonBool::setValue(bool value)
{
	mValue = value;
}

/**
 * Increases the reference count by one.
 * Reference count is used to monitor/handle the reading
 * and writing of the object.
 */

void JsonBool::ref()
{
	mRefcount++;
}

/**
 * Decreases the reference count by one.
 * Reference count is used to monitor/handle the reading
 * and writing of the object.
 */

void JsonBool::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

/**
 * Returns the reference count.
 *
 * @return	reference count as Size32.
 * @see Stateplex::Size32
 */

Stateplex::Size32 JsonBool::refcount() 
{
	return mRefcount;
}

/**
 * Default destructor for JsonBool.
 */

JsonBool::~JsonBool()
{ }
