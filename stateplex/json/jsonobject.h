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
	JsonItem *find(const char *target);
	JsonObject *findObject(const char *target);
	const char *key();
	const char *type();
protected:
private:
	void freeItems();
	
	Stateplex::List<JsonItem> *mItems;
	const char *mKey;
};

#endif // JSONOBJECT_H
