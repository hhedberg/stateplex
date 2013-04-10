#ifndef INCLUDED_JSON_DB_ACTOR_H
#define INCLUDED_JSON_DB_ACTOR_H

#include "../core/callbackmessage.h"
#include "../core/actor.h"
#include "../core/string.h"
#include "jsonitem.h"

enum JsonMessageType {
	JSON_MESSAGE_TYPE_GET,
	JSON_MESSAGE_TYPE_SET
};

class JsonDbActor : public Stateplex::Actor
{
public:

	JsonDbActor(Stateplex::Dispatcher*);

	class JsonMessage : public Stateplex::CallbackMessage<JsonDbActor, JsonMessage> {
		JsonMessageType mType;
		//String *mParameter;
		JsonItem *mResult;

	public:


		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, JsonItem *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, void (Sender::*callbackFunction)(JsonMessage *message));
		void result() const;

	protected:
		void handle(Actor *sender, JsonDbActor *receiver);
		
	};

	template<typename Sender> void getRootObject(Sender *sender, JsonItem *item, void (Sender::*callbackFunction)(JsonMessage *message));
};

/*** Inline implementations ***/

#include <iostream>

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(Sender *sender, JsonDbActor *receiver, JsonItem *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item)
{ }

inline void JsonDbActor::JsonMessage::handle(Actor *sender, JsonDbActor *receiver)
{	
	JsonString *seppo = reinterpret_cast<JsonString *> (mResult);
	std::cout << "TEST: " << seppo->key();
	invokeCallback();
}

inline void JsonDbActor::JsonMessage::result() const
{
	std::cout << "TESTI:";
}

inline JsonDbActor::JsonDbActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

template<typename Sender> void JsonDbActor::getRootObject(Sender *sender, JsonItem *item, void (Sender::*callbackFunction)(JsonMessage *message))
{
	JsonMessage *rootObject = new JsonMessage(sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

#endif
