#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"


class JsonObject : public JsonItem
{
public:
        JsonObject();
        virtual ~JsonObject();
	void add(JsonItem *item);
	void traverse();
	void freeItems();
protected:
private:
	Stateplex::List<JsonItem> mItems;
};

#endif // JSONOBJECT_H
