#ifndef INCLUDED_JSON_CLIENT_H
#define INCLUDED_JSON_CLIENT_H

#include <stateplex/core/actor.h>
#include <stateplex/core/callbackmessage.h>
#include "jsoninterface.h"
#include <stateplex/json/jsondbactor.h>
#include "jsonadapter.h"

class JsonClient : public Stateplex::Actor {
	
	JsonAdapter<JsonClient, JsonDbActor::JsonMessage> *mSource;

public:
	JsonClient(Stateplex::Dispatcher *dispatcher);

	void showResult(JsonDbActor::JsonMessage *message);
	void plugin(JsonAdapter<JsonClient, JsonDbActor::JsonMessage> *source);
	template<class T, class M> void get(const char *path, JsonObject *item, void(T::*function)(M *message));
	template<class T, class M> void set(const char *path, const char *parameter, JsonObject *item, void(T::*function)(M *message));
	template<class T, class M> void add(const char *path, JsonItem *newItem, JsonObject *item, void(T::*function)(M *message));
};

/*** Inline implementations ***/


JsonClient::JsonClient(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

void JsonClient::showResult(JsonDbActor::JsonMessage *message)
{
	if(message != NULL) {
		message->result();
	}
}

inline void JsonClient::plugin(JsonAdapter<JsonClient, JsonDbActor::JsonMessage> *source)
{
	mSource = source;
}

template<class T, class M>
inline void JsonClient::get(const char *path, JsonObject *item, void(T::*function)(M *message))
{
	mSource->get(path, item, this, function);
}

template<class T, class M>
inline void JsonClient::set(const char *path, const char *parameter, JsonObject *item, void(T::*function)(M *message))
{
	mSource->set(path, parameter, item, this, function);
}

template<class T, class M>
inline void JsonClient::add(const char *path, JsonItem *newItem, JsonObject *item, void(T::*function)(M *message))
{
	mSource->add(path, newItem, item, this, function);
}


#endif
