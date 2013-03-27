#include "jsonstring.h"

JsonString::JsonString()
{
        //ctor
}

JsonString::JsonString(const char *key, const char *value)
{
	mKey = key;
	mValue = value;
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

JsonString::~JsonString()
{
        //dtor
}
