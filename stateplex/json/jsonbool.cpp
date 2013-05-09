#include "jsonbool.h"
#include <iostream>

JsonBool::JsonBool(Stateplex::Actor *owner, const char *key, bool b)
	: JsonItem(owner)
{
	mKey = const_cast<char*>(key);
	mValue = b;
	mType = JSON_BOOL;
}

void JsonBool::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

bool JsonBool::value()
{
	return mValue;
}

const char *JsonBool::key()
{
	return const_cast<const char*>(mKey);
}

JsonType JsonBool::type()
{
	return mType;
}


void JsonBool::setValue(bool value)
{
	mValue = value;
}

void JsonBool::ref()
{
	mRefcount++;
}

void JsonBool::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

Stateplex::Size32 JsonBool::refcount() 
{
	return mRefcount;
}

JsonBool::~JsonBool()
{
        //dtor
}
