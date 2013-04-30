#ifndef JSONADAPTER_H
#define JSONADAPTER_H

#include "../../examples/jsonhello/jsoninterface.h"
#include "jsondbactor.h"

class Sender;

class JsonAdapter : public JsonInterface
{
public:
	JsonAdapter();
	~JsonAdapter();
	void set(const char *path, const char *value, JsonObject *item);
	void get(const char *path, JsonObject *item);
	void add(const char *path, JsonItem *newItem, JsonObject *item);
	
	void plugin(JsonDbActor *source);
private:
	JsonDbActor *mSource;

protected:
	
};




/*** Inline implementation ***/

inline void JsonAdapter::set(const char *path, const char *parameter, JsonObject *item)
{
	mSource->setJsonObject(path, parameter, item);
}

inline void JsonAdapter::get(const char *path, JsonObject *item)
{
	mSource->getRootObject(path, item);
}

inline void JsonAdapter::add(const char *path, JsonItem *newItem, JsonObject *item)
{
	mSource->addJson(path, newItem, item);
}

inline void JsonAdapter::plugin(JsonDbActor *source)
{
	mSource = source;
}

JsonAdapter::JsonAdapter() 
{ 
	//ctor
}
JsonAdapter::~JsonAdapter()
{
	//dtor
}


#endif
