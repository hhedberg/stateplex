#ifndef INCLUDED_JSON_DB_ACTOR_H
#define INCLUDED_JSON_DB_ACTOR_H

#include "../core/callbackmessage.h"
#include "../core/actor.h"
#include "../core/string.h"
#include "jsonobject.h"

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
		Stateplex::String *mPath;
		Stateplex::String *mParameter;
		JsonItem *mResult;

	public:


		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Stateplex::String *parameter, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, void (Sender::*callbackFunction)(JsonMessage *message));
		void result() const;

	protected:
		void handle(Actor *sender, JsonDbActor *receiver);
		
	};

	template<typename Sender> void getRootObject(const char *path, Sender *sender, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
	template<typename Sender> void setJsonObject(const char *path, const char *parameter, Sender *sender, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
};

/*** Inline implementations ***/

#include <iostream>

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, Stateplex::String *parameter, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type),mPath(path),  mParameter(parameter)
{ }

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type), mPath(path)
{ }

inline void JsonDbActor::JsonMessage::handle(Actor *sender, JsonDbActor *receiver)
{	
	JsonItem *test = NULL;
	if(mType == JSON_MESSAGE_TYPE_GET) {
		test = dynamic_cast<JsonObject *>(mResult)->get(mPath);
	} else {
		test = dynamic_cast<JsonObject *>(mResult)->set(mPath, mParameter);
	}
	mResult = dynamic_cast<JsonItem *>(test);
	
	if(test == NULL) {
		invokeCallback(NULL);
	} else {
		invokeCallback(this);
	}
}

inline void JsonDbActor::JsonMessage::result() const
{
	if(mResult->type() == JSON_OBJECT) {
		std::cout << "Key: " << mResult->key() << std::endl;
	} else if(mResult->type() == JSON_STRING) {
		std::cout << "Key: " << mResult->key() << std::endl;
		std::cout << "Value: " << dynamic_cast<JsonString *> (mResult)->value() << std::endl;
	} else if(mResult->type() == JSON_NUMBER) {
		std::cout << "Key: " << mResult->key() << std::endl;
		std::cout << "Value: " << dynamic_cast<JsonNumber *> (mResult)->value() << std::endl;
	}
	
}

inline JsonDbActor::JsonDbActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

template<typename Sender> void JsonDbActor::getRootObject(const char *path, Sender *sender, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_GET, Stateplex::String::copy(allocator(), path), sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

template<typename Sender> void JsonDbActor::setJsonObject(const char *path, const char *parameter, Sender *sender, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_SET, Stateplex::String::copy(allocator(), path), Stateplex::String::copy(allocator(), parameter), sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

#endif
