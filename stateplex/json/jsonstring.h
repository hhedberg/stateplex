#ifndef JSONSTRING_H
#define JSONSTRING_H

#include "jsonitem.h"
#include <iostream>

class JsonString : public JsonItem
{
public:
	JsonString(Stateplex::Actor *owner, const char *key, const char *value);
        virtual ~JsonString();
	void traverse();
	const char *key();
	const char *value();
	void setValue(Stateplex::String *str);
	JsonType type();
	void unref();
	Stateplex::Size32 refcount();
	
protected:
	void ref();
	
private:
	const char *mValue;
};

#endif // JSONSTRING_H
