#include "jsonobject.h"
#include <iostream>

JsonObject::JsonObject(const char *key)
{
        mKey = key;
	mItems = new Stateplex::List<JsonItem>;
	mType = "JSON_OBJECT";
}

JsonObject::~JsonObject()
{
        freeItems();
	delete mItems;
}

void JsonObject::add(JsonItem *item)
{
	mItems->addTail(item);
}

void JsonObject::traverse()
{
	std::cout << '"' <<  key() << '"' << " : " << "{" << std::endl;
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(iterator.hasSubsequent()) {
			std::cout < "\t";
		}
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		item->traverse();
	}

	std::cout << "\t" << "}" << std::endl;

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

const char *JsonObject::type()
{
	return mType;
}

void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		delete item;
	}

}

JsonItem *JsonObject::find(const char *target)
{
	JsonItem *item;
	
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(strcmp(iterator.current()->key(), target) == 0) {
			item = iterator.current();
			break;
		}
	}

	return item;
}

JsonObject *JsonObject::findObject(const char *target)
{
	JsonItem *item = find(target);

	if(strcmp(item->type(), "JSON_OBJECT") != 0) {
		return 0;
	}

	return reinterpret_cast<JsonObject *> (item);
}
