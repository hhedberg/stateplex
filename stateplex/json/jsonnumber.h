#ifndef JSONNUMBER_H
#define JSONNUMBER_H

#include "jsonitem.h"
#include <iostream>

/**
 * @brief JsonNumbers contains non-decimal numbers.
 *
 * Class JsonNumber is a sub class of JsonItem. JsonNumber
 * is a json item that can contain a non decimal number value.
 * JsonNumber content can be modified after initialization.
 */

class JsonNumber : public JsonItem
{
public:
	JsonNumber(Stateplex::Actor *owner, const char *key, int number);
        virtual ~JsonNumber();
	const char *key();
	int value();
	void setValue(int value);
	void unref();
	void traverse();
	JsonType type();
	Stateplex::Size32 refcount();
protected:
	void ref();
private:
	int mValue;
};

#endif // JSONNUMBER_H
