#include "jsonstring.h"

JsonString::JsonString()
{
        //ctor
}

JsonString::JsonString(std::string value)
{
	mString = value;
}

void JsonString::traverse()
{
	std::cout << mString << ' ';
}

JsonString::~JsonString()
{
        //dtor
}
