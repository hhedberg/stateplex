#ifndef INCLUDED_JSON_CLIENT_H
#define INCLUDED_JSON_CLIENT_H

#include <stateplex/core/actor.h>
#include <stateplex/core/callbackmessage.h>
#include "jsoninterface.h"
#include <stateplex/json/jsondbactor.h>

class JsonClient : public Stateplex::Actor {
	
	JsonInterface *mSource;

public:
	JsonClient(Stateplex::Dispatcher *dispatcher);

	void showResult(JsonDbActor::JsonMessage *message);
	void plugin(JsonInterface *source);
	virtual void get(const char *path, JsonObject *item);
	virtual void set(const char *path, const char *parameter, JsonObject *item);
	virtual void add(const char *path, JsonItem *newItem, JsonObject *item);
};

/*** Inline implementations ***/


JsonClient::JsonClient(Stateplex::Dispatcher *dispatcher)
	: Object(this), Actor(dispatcher)
{ }

void JsonClient::showResult(JsonDbActor::JsonMessage *message)
{
	if(message != NULL) {
		message->result();
	}
}

inline void JsonClient::plugin(JsonInterface *source)
{
	mSource = source;
}

inline void JsonClient::get(const char *path, JsonObject *item)
{
	mSource->get(path, item);
}

inline void JsonClient::set(const char *path, const char *parameter, JsonObject *item)
{
	mSource->set(path, parameter, item);
}

inline void JsonClient::add(const char *path, JsonItem *newItem, JsonObject *item)
{
	mSource->add(path, newItem, item);
}


#endif
