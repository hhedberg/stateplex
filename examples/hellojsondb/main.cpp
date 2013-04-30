/*
 * Stateplex - A server-side actor model library.
 *
 * examples/hellojsondb/main.cpp
 *
 * (c) 2013 Henrik Hedberg <henrik.hedberg@innologies.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Authors: Henrik Hedberg
 */

#include <iostream>

#include <stateplex/core/dispatcher.h>
#include <stateplex/jsondb/jsondbactor.h>
#include <stateplex/jsondb/jsonobject.h>
#include <stateplex/core/string.h>

using namespace Stateplex;

class JsonObserver : public JsonItem::Observer {
	virtual void itemDeleted(JsonItem *item);
	virtual void elementInserted(JsonArray *array, int index);
	virtual void elementRemoved(JsonArray *array, int index);
	virtual void memberSet(JsonObject *object, const String *name);
	virtual void memberUnset(JsonObject *object, const String *name);
};

int main(void)
{
	Dispatcher dispatcher;
	JsonDbActor db(&dispatcher);

	String *name = String::copy(dispatcher.allocator(), "test");
	db.createRoot(name);
	JsonObject *root = db.root(name);
	name->destroy(dispatcher.allocator());

	JsonObserver observer;
	root->addObserver(&observer);

	String *set1 = String::copy(dispatcher.allocator(), "member");
	String *get1 = String::copy(dispatcher.allocator(), "member");

	root->setDecimal(set1, 12.3);

	std::cout << root->decimal(get1) << std::endl;

	JsonObject *object1 = root->setObject(set1);
	object1->setInteger(set1, 23);

	std::cout << root->decimal(get1) << std::endl;

	JsonObject *object2 = root->object(get1);
	std::cout << object2->integer(get1) << std::endl;

	JsonArray *array1 = root->setArray(set1);
	String *string = String::copy(dispatcher.allocator(), "value");
	array1->appendString(string);

	JsonObject *object3 = root->object(get1);

	std::cout << object3 << std::endl;

	JsonArray *array2 = root->array(get1);

	std::cout << "*" << array2 << std::endl;
	std::cout << array2->length() << std::endl;
	std::cout << array2->string(0)->chars() << std::endl;

	set1->destroy(dispatcher.allocator());
	get1->destroy(dispatcher.allocator());
	string->destroy(dispatcher.allocator());

	root->removeObserver(&observer);

	return 0;
}

void JsonObserver::itemDeleted(JsonItem *item)
{
	std::cout << "Item deleted: " << item << std::endl;
}

void JsonObserver::elementInserted(JsonArray *array, int index)
{
	std::cout << "Element inserted: " << index << std::endl;
}

void JsonObserver::elementRemoved(JsonArray *array, int index)
{
	std::cout << "Element removed: " << index << std::endl;
}

void JsonObserver::memberSet(JsonObject *object, const String *name)
{
	std::cout << "Member set: " << name->chars() << std::endl;
}
void JsonObserver::memberUnset(JsonObject *object, const String *name)
{
	std::cout << "member unset: " << name->chars() << std::endl;
}
