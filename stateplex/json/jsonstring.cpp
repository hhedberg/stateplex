#include "jsonstring.h"

JsonString::JsonString()
{
        //ctor
}

JsonString::JsonString(const char *key, const char *value)
{
	mKey = key;
	mValue = value;
	mType = JSON_STRING;
}

void JsonString::traverse()
{
	std::cout << mValue << ' ';
}

const char *JsonString::value()
{
	return mValue;
}

const char *JsonString::key()
{
	return mKey;
}

JsonType JsonString::type()
{
	return mType;
}

JsonString::~JsonString()
{
        //dtor
}
