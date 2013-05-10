#include "jsonstring.h"

/**
 * Constructor that assigns the owner of the 
 * jsonstring and assigns the key and value for it.
 *
 * @param *owner	pointer to the owner actor.
 * @param *key		wanted value for the key.
 * @param *value	wanted value for the value.
 */

JsonString::JsonString(Stateplex::Actor *owner, const char *key, const char *value)
	: JsonItem(owner)
{
	mKey = const_cast<char *> (key);
	mValue = value;
	mType = JSON_STRING;
}

/**
 * Prints out the value of the json string.
 * 
 * @see 		JsonObject::traverse()
 */

void JsonString::traverse()
{
	std::cout << mValue << ' ';
}

/**
 * Returns the value of the json string.
 *
 * @return		value of json string.
 */

const char *JsonString::value()
{
	return mValue;
}

/**
 * Returns the key of the json string.
 *
 * @return		key of json string.
 */

const char *JsonString::key()
{
	return mKey;
}

/**
 * Returns the type of json string.
 * can be used to compare that it is
 * actually a json string.
 *
 * @return		type of the jsonstring object.
 */

JsonType JsonString::type()
{
	return mType;
}

/**
 * Takes given Stateplex::String as parameter
 * and assigns it's value to json string's value.
 *
 * @param *str		value to set.
 * @see 		Stateplex::String.
 */

void JsonString::setValue(Stateplex::String *str)
{
	mValue = str->chars();
}

/**
 * Increases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonString.
 */

void JsonString::ref()
{
	mRefcount++;
}

/**
 * Decreases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonString.
 */

void JsonString::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

/**
 * Returns the reference count
 *
 * @return		reference count as Size32.
 * @see 		Stateplex::Size32.
 */

Stateplex::Size32 JsonString::refcount() 
{
	return mRefcount;
}

/**
 * Default destructor for JsonString.
 */

JsonString::~JsonString()
{ }

