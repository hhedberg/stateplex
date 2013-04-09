#ifndef JSONNUMBER_H
#define JSONNUMBER_H

#include "jsonitem.h"

class JsonNumber : public JsonItem
{
public:
	JsonNumber();
	JsonNumber(const char *key, int number);
        virtual ~JsonNumber();
	void traverse();
	const char *key();
	const char *type();
	int value();
protected:
private:
	int mValue;
	//const char *mKey;
};

#endif // JSONNUMBER_H
