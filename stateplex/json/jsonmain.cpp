#include "jsonitem.h"
#include "jsonnumber.h"
#include "jsonobject.h"
#include <iostream>

int main()
{
	Stateplex::List<JsonObject> mList;
	JsonObject mObjects;

	for (int i = 0; i < 12; i++) {
		mObjects.add(new JsonNumber("seppo", i *3));
	}

	mList.addTail(&mObjects);

	for (Stateplex::ListIterator<JsonObject> iterator(&mList); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = iterator.current();
		item->traverse();
	}

	std::cout << std::endl;

	for (Stateplex::ListIterator<JsonObject> iterator(&mList); iterator.hasCurrent(); iterator.subsequent()) {
			JsonObject *item = iterator.current();
			item->freeItems();
	}

  return 0;
}


