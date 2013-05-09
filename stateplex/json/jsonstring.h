#ifndef JSONSTRING_H
#define JSONSTRING_H

#include "jsonitem.h"

/**
 * @brief JsonStrings contains string data.
 *
 * Class JsonString is a sub class of JsonItem. JsonString
 * is a json item that can contain different kind of characters.
 * JsonString content can be modified after initialization.
 */

class JsonString : public JsonItem
{
public:
	JsonString(Stateplex::Actor *owner, const char *key, const char *value);
        virtual ~JsonString();
	const char *key();
	const char *value();
	void setValue(Stateplex::String *str);
	void traverse();
	void unref();
	JsonType type();
	Stateplex::Size32 refcount();
	
protected:
	void ref();
private:
	const char *mValue;
};

#endif // JSONSTRING_H
