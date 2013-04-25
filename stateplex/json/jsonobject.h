#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"
#include "jsonnumber.h"
#include "jsonstring.h"
#include "jsondata.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
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
	JsonItem *get(Stateplex::String *path);
	JsonObject *set(Stateplex::String *path, Stateplex::String *parameter);
	const char *key();
	void setKey(Stateplex::String *str);
	JsonType type();
	void unref();

protected:
	void ref();
	
private:
	void freeItems();
	void tokenizepath(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = "/");
	bool isInteger(Stateplex::String *s);
	int toInteger(Stateplex::String *s);
	
	Stateplex::List<JsonItem> *mItems;
};

#endif // JSONOBJECT_H
