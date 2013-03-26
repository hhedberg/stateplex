#include "jsonobject.h"

JsonObject::JsonObject()
{
        //ctor
}

JsonObject::~JsonObject()
{
        //dtor
}

void JsonObject::add(JsonItem *item)
{
	mItems.addTail(item);
}

void JsonObject::traverse()
{
	for (Stateplex::ListIterator<JsonItem> iterator(&mItems); iterator.hasCurrent(); iterator.subsequent()) {
				JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
				item->traverse();
	}

}

void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(&mItems); iterator.hasCurrent(); iterator.subsequent()) {
				JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
				delete item;
	}

}
