#ifndef INCLUDED_JSON_CLIENT_H
#define INCLUDED_JSON_CLIENT_H

#include <stateplex/core/actor.h>
#include <stateplex/core/callbackmessage.h>

#include <stateplex/json/jsondbactor.h>

class JsonClient : public Stateplex::Actor {

public:
	JsonClient(Stateplex::Dispatcher *dispatcher);

	void showResult(JsonDbActor::JsonMessage *message);
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

#endif
