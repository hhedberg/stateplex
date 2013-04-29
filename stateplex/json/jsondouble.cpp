#include "jsondouble.h"
#include <iostream>

JsonDouble::JsonDouble(Stateplex::Actor *owner, const char *key, double number)
	: JsonItem(owner)
{
	mKey = const_cast<char*>(key);
	mValue = number;
	mType = JSON_DOUBLE;
}

void JsonDouble::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

double JsonDouble::value()
{
	return mValue;
}

const char *JsonDouble::key()
{
	return const_cast<const char*>(mKey);
}

JsonType JsonDouble::type()
{
	return mType;
}


void JsonDouble::setValue(double value)
{
	mValue = value;
}

void JsonDouble::ref()
{
	mRefcount++;
}

void JsonDouble::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

JsonDouble::~JsonDouble()
{
        //dtor
}
