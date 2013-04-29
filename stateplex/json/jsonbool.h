#ifndef JSONBOOL_H
#define JSONBOOL_H

#include "jsonitem.h"

class JsonBool : public JsonItem
{
public:
	JsonBool(Stateplex::Actor *owner, const char *key, bool b);
        virtual ~JsonBool();
	void traverse();
	const char *key();
	JsonType type();
	bool value();
	void setValue(bool value);
	void unref();
	
protected:
	void ref();
	
private:
	bool mValue;
};

#endif // JSONBOOL_H
