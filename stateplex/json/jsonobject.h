#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"
#include <string>
#include <vector>
#include <iostream>

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
	JsonObject *get(Stateplex::String *path);
	JsonObject *set(Stateplex::String *path);
	const char *key();
	JsonType type();

protected:
private:
	void freeItems();
	void tokenizepath(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = "/");
	
	Stateplex::List<JsonItem> *mItems;
};

#endif // JSONOBJECT_H
