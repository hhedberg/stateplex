#include "jsonnumber.h"
#include <iostream>

JsonNumber::JsonNumber()
{
        //ctor
}

JsonNumber::JsonNumber(const char *key, int number)
{
	mKey = key;
	mValue = number;
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

JsonNumber::~JsonNumber()
{
        //dtor
}
