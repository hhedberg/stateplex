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
protected:
private:
	int mNumber;
	const char *mKey;
};

#endif // JSONNUMBER_H
