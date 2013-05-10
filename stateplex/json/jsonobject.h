#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "jsonitem.h"
#include "jsonnumber.h"
#include "jsonstring.h"
#include "jsonbool.h"
#include "jsondouble.h"
#include <string>
#include <vector>
#include <sstream>
#include <ctype.h>

/**
 * @brief JsonObject contains JsonItems.
 *
 * Class JsonObject is a sub class of JsonItem. JsonObject
 * contains other JsonItems.
 * JsonObject content can be modified after initialization.
 */
 
class JsonObject : public JsonItem
{
public:
    	JsonObject(Stateplex::Actor *owner, const char *key);
    	virtual ~JsonObject();
	const char *key();
 	void add(JsonItem *item);
	void add(Stateplex::String *path, JsonItem *item);
	void traverse();
	void setKey(Stateplex::String *str);
	void unref();
	JsonItem *find(const char *target);
	JsonItem *get(Stateplex::String *path);
	JsonItem *get(const char *path);
	JsonObject *findObject(const char *target);
	JsonObject *getObject(const char *path);
	JsonObject *set(Stateplex::String *path, Stateplex::String *parameter);
	JsonType type();
	Stateplex::Size32 refcount();
	Stateplex::List<JsonItem> *getList();
protected:
	void ref();
private:
	void freeItems();
	void tokenizepath(const std::string& str, std::vector<std::string>& tokens, const char delimiter = '/');
	bool isInteger(Stateplex::String *s);
	int toInteger(Stateplex::String *s);
	bool isDouble(Stateplex::String *str);
	double toDouble(Stateplex::String *str);
	
	Stateplex::List<JsonItem> *mItems;
};

#endif // JSONOBJECT_H
