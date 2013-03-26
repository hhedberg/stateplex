#include "jsonnumber.h"
#include <iostream>

JsonNumber::JsonNumber()
{
        //ctor
}

JsonNumber::JsonNumber(const char* key, int number)
{
	mKey = key;
	mNumber = number;
}

void JsonNumber::traverse()
{
	std::cout << mKey << " : " << mNumber << std::endl;
}

JsonNumber::~JsonNumber()
{
        //dtor
}
