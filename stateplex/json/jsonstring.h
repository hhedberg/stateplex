#ifndef JSONSTRING_H
#define JSONSTRING_H

#include "jsonitem.h"
#include <string>
#include <iostream>

class JsonString : public JsonItem
{
public:
	JsonString();
	JsonString(const char *key, const char *value);
        virtual ~JsonString();
	void traverse();
	const char *key();
	const char *value();
	const char *type();
protected:
private:
	const char *mValue;
	//const char *mKey;
};

#endif // JSONSTRING_H
