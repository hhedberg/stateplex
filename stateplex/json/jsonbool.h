#ifndef JSONBOOL_H
#define JSONBOOL_H

#include "jsonitem.h"

/**
 * @brief JsonBool contains boolean data
 *
 * Class JsonBool is a sub class of JsonItem. JsonBool
 * is a json item that contains boolean data (true or false).
 * JsonBool content can be modified after initialization.
 */

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
	Stateplex::Size32 refcount();
	
protected:
	void ref();
	
private:
	bool mValue;
};

#endif // JSONBOOL_H
