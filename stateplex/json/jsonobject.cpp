#include "jsonobject.h"
#include <iostream>

JsonObject::JsonObject(const char *key)
{
    mKey = key;
	mItems = new Stateplex::List<JsonItem>;
	mType = JSON_OBJECT;
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
	int count = 0;
	std::cout << '"' <<  key() << '"' << " : " << "{" << std::endl;
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(iterator.hasSubsequent()) {
			for(int i = 0; i < count; i++) {
				std::cout < "\t";	
			}
		}
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		item->traverse();
	}

	std::cout << "}" << std::endl;

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

JsonType JsonObject::type()
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

	if(item->type() != JSON_OBJECT) {
		return 0;
	}

	return dynamic_cast<JsonObject *> (item);
}
