#ifndef INCLUDED_JSON_DB_ACTOR_H
#define INCLUDED_JSON_DB_ACTOR_H

#include "../core/callbackmessage.h"
#include "../core/actor.h"
#include "../core/string.h"
#include "jsonitem.h"

class JsonDbActor : public Stateplex::Actor
{



public:

	JsonDbActor();
	~JsonDbActor();

	class JsonMessage: public Stateplex::CallbackMessage<JsonDbActor, JsonMessage> {
		JsonMessageType mType;
		//String *mParameter;
		JsonItem *mResult;

	public:
		enum JsonMessageType {
			JSON_MESSAGE_TYPE_GET,
			JSON_MESSAGE_TYPE_SET
		};

		JsonMessage();
		~JsonMessage();

	protected:
		void handle(Actor *sender, Actor *receiver);
	};


};

#endif