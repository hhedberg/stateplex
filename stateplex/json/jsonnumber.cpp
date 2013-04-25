#include "jsonnumber.h"
#include <iostream>

JsonNumber::JsonNumber(Stateplex::Actor *owner, const char *key, int number)
	: JsonItem(owner)
{
	mKey = key;
	mValue = number;
	mType = JSON_NUMBER;
}

void JsonNumber::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

int JsonNumber::value()
{
	return mValue;
}

const char *JsonNumber::key()
{
	return mKey;
}

JsonType JsonNumber::type()
{
	return mType;
}

void JsonNumber::ref()
{
	mRefcount++;
}

void JsonNumber::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

JsonNumber::~JsonNumber()
{
        //dtor
}
