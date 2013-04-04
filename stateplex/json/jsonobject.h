#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"
#include "jsonnumber.h"

class JsonObject : public JsonItem
{
public:
        JsonObject(const char *key);
        virtual ~JsonObject();
	void add(JsonItem *item);
	void traverse();
	void freeItems();
	//JsonNumber *nextJson();
	Stateplex::List<JsonItem> *getList();
	JsonItem *getJson(const char *target);
	const char *key();
protected:
private:
	Stateplex::List<JsonItem> *mItems;
	const char *mKey;
};

#endif // JSONOBJECT_H
