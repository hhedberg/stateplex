#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"

class JsonObject : public JsonItem
{
public:
    	JsonObject(Stateplex::Actor *owner, const char *key);
    	virtual ~JsonObject();
 	void add(JsonItem *item);
	void traverse();
	Stateplex::List<JsonItem> *getList();
	JsonItem *find(const char *target);
	JsonObject *findObject(const char *target);
	const char *key();
	JsonType type();
protected:
private:
	void freeItems();
	
	Stateplex::List<JsonItem> *mItems;
};

#endif // JSONOBJECT_H
