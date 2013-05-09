#include "jsonnumber.h"

/**
 * Constructor that assigns the owner of the 
 * jsonstring and assigns the key and value for it.
 *
 * @param *owner	pointer to the owner actor.
 * @param *key		wanted value for the key.
 * @param *number	wanted number for the value.
 */

JsonNumber::JsonNumber(Stateplex::Actor *owner, const char *key, int number)
	: JsonItem(owner)
{
	mKey = const_cast<char*>(key);
	mValue = number;
	mType = JSON_NUMBER;
}

/**
 * Prints out the value of the json number.
 * 
 * @see 		JsonObject::traverse()
 */

void JsonNumber::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

/**
 * Returns the value of the json number.
 *
 * @return		value of json number.
 */

int JsonNumber::value()
{
	return mValue;
}

/**
 * Returns the key of the json number.
 *
 * @return		key of json number.
 */

const char *JsonNumber::key()
{
	return const_cast<const char*>(mKey);
}

/**
 * Returns the type of json number.
 * can be used to compare that it is
 * actually a json number.
 *
 * @return		type of the jsonnumber object.
 */

JsonType JsonNumber::type()
{
	return mType;
}

/**
 * Takes given number as parameter
 * and assigns it's value to json number's value.
 *
 * @param value		number to set.
 */

void JsonNumber::setValue(int value)
{
	mValue = value;
}

/**
 * Increases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonNumber.
 */

void JsonNumber::ref()
{
	mRefcount++;
}

/**
 * Decreases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonNumber.
 */

void JsonNumber::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

/**
 * Returns the reference count.
 *
 * @return		reference count as Size32.
 * @see 		Stateplex::Size32.
 */

Stateplex::Size32 JsonNumber::refcount() 
{
	return mRefcount;
}

/**
 * Default destructor for JsonNumber.
 */

JsonNumber::~JsonNumber()
{ }
