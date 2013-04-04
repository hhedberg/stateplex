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
	//JsonNumber *nextJson();
	Stateplex::List<JsonItem> *getList();
	const char *key();
protected:
private:
	void freeItems();
	
	Stateplex::List<JsonItem> *mItems;
	const char *mKey;
};

#endif // JSONOBJECT_H
