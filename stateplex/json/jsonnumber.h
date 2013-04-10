#ifndef JSONNUMBER_H
#define JSONNUMBER_H

#include "jsonitem.h"

class JsonNumber : public JsonItem
{
public:
	JsonNumber(Stateplex::Actor *owner, const char *key, int number);
        virtual ~JsonNumber();
	void traverse();
	const char *key();
	JsonType type();
	int value();
protected:
private:
	int mValue;
};

#endif // JSONNUMBER_H
