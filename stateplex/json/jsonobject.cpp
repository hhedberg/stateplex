#include "jsonobject.h"

JsonObject::JsonObject(const char *key)
{
        mKey = key;
	mItems = new Stateplex::List<JsonItem>;
}

JsonObject::~JsonObject()
{
        //dtor
}

void JsonObject::add(JsonItem *item)
{
	mItems->addTail(item);
}

void JsonObject::traverse()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		item->traverse();
	}

}

/*JsonNumber *JsonObject::nextJson()
{
	return mIterator.subsequent();
}*/

Stateplex::List<JsonItem> *JsonObject::getList()
{
	return mItems;
}

const char *JsonObject::key()
{
	return mKey;
}

void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		delete item;
	}

}
