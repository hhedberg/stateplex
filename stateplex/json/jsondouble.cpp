#include "jsondouble.h"

/**
 * Constructor that takes assigns the owner of the 
 * jsonstring and assigns the key and value for it.
 *
 * @param *owner	pointer to the owner actor.
 * @param *key		wanted value for the key.
 * @param number	wanted double for the value.
 */

JsonDouble::JsonDouble(Stateplex::Actor *owner, const char *key, double number)
	: JsonItem(owner)
{
	mKey = const_cast<char*>(key);
	mValue = number;
	mType = JSON_DOUBLE;
}

/**
 * Prints out the value of the json double.
 * 
 * @see JsonObject::traverse()
 */

void JsonDouble::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

/**
 * Returns the value of the json double.
 *
 * @return		value of json double.
 */

double JsonDouble::value()
{
	return mValue;
}

/**
 * Returns the key of the json double.
 *
 * @return		key of json double.
 */

const char *JsonDouble::key()
{
	return const_cast<const char*>(mKey);
}

/**
 * Returns the type of json double.
 * Can be used to compare that it is
 * actually a json double.
 *
 * @return		type of the jsondouble object.
 */

JsonType JsonDouble::type()
{
	return mType;
}

/**
 * Takes given decimal value as parameter
 * and assigns it's value to json double's value.
 *
 * @param value		decimal to set.
 */

void JsonDouble::setValue(double value)
{
	mValue = value;
}

/**
 * Increases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonDouble.
 */

void JsonDouble::ref()
{
	mRefcount++;
}

/**
 * Decreases the reference count by one.
 * Reference count is used to monitor/handle the usage
 * of the JsonDouble.
 */

void JsonDouble::unref()
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

Stateplex::Size32 JsonDouble::refcount() 
{
	return mRefcount;
}

/**
 * Default destructor for JsonDouble.
 */

JsonDouble::~JsonDouble()
{ }
