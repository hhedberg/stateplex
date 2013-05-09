#ifndef JSONDOUBLE_H
#define JSONDOUBLE_H

#include "jsonitem.h"

class JsonDouble : public JsonItem
{
public:
	JsonDouble(Stateplex::Actor *owner, const char *key, double number);
        virtual ~JsonDouble();
	void traverse();
	const char *key();
	JsonType type();
	double value();
	void setValue(double value);
	void unref();
	Stateplex::Size32 refcount();
	
protected:
	void ref();
	
private:
	double mValue;
};

#endif // JSONDOUBLE_H
