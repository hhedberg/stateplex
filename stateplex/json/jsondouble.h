#ifndef JSONDOUBLE_H
#define JSONDOUBLE_H

#include "jsonitem.h"

/**
 * @brief JsonDouble contains double data.
 *
 * Class JsonDouble is a sub class of JsonItem. JsonDouble
 * is a json item that can contain a decimal value.
 * JsonDouble content can be modified after initialization.
 */

class JsonDouble : public JsonItem
{
public:
	JsonDouble(Stateplex::Actor *owner, const char *key, double number);
        virtual ~JsonDouble();	
	const char *key();
	double value();
	void setValue(double value);
	void unref();
	void traverse();
	JsonType type();
	Stateplex::Size32 refcount();
protected:
	void ref();
private:
	double mValue;
};

#endif // JSONDOUBLE_H
