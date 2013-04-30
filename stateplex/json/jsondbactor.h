#ifndef INCLUDED_JSON_DB_ACTOR_H
#define INCLUDED_JSON_DB_ACTOR_H

#include "../core/callbackmessage.h"
#include "../core/actor.h"
#include "../core/string.h"
#include "jsonobject.h"

class Sender;

enum JsonMessageType {
	JSON_MESSAGE_TYPE_GET,
	JSON_MESSAGE_TYPE_SET,
	JSON_MESSAGE_TYPE_ADD
};

class JsonDbActor : public Stateplex::Actor
{
public:

	JsonDbActor(Stateplex::Dispatcher*);

	class JsonMessage : public Stateplex::CallbackMessage<JsonDbActor, JsonMessage> {
		JsonMessageType mType;
		Stateplex::String *mPath;
		Stateplex::String *mParameter;
		JsonItem *mNew;
		JsonItem *mResult;

	public:

		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, JsonItem *newItem, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Stateplex::String *parameter, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(Sender *sender, JsonDbActor *receiver, void (Sender::*callbackFunction)(JsonMessage *message));
		void result() const;

	protected:
		void handle(Actor *sender, JsonDbActor *receiver);
		
	};
	
	Sender *mSender;
	void (Sender::*mCallbackFunction)(JsonDbActor::JsonMessage *message);
	
	void getRootObject(const char *path, JsonObject *item);
	void setJsonObject(const char *path, const char *parameter, JsonObject *item);
	void addJson(const char *path, JsonItem *newItem, JsonObject *item);
	template<typename Sender> void setClient(Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message));
};

/*** Inline implementations ***/

#include <iostream>

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, JsonItem *newItem, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type),mPath(path), mNew(newItem)
{ }

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
	} else if (mType == JSON_MESSAGE_TYPE_SET) {
		test = dynamic_cast<JsonObject *>(mResult)->set(mPath, mParameter);
	} else if (mType == JSON_MESSAGE_TYPE_ADD) {
		char *tmp = mPath->chars();
		dynamic_cast<JsonObject *>(mResult)->add(mPath, mNew);
		strcat(tmp, mNew->key());
		strcat(tmp, "/");
		test = dynamic_cast<JsonObject *>(mResult)->get(tmp);
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
	} else if(mResult->type() == JSON_DOUBLE) {
		std::cout << "Key: " << mResult->key() << std::endl;
		std::cout << "Value: " << dynamic_cast<JsonDouble *> (mResult)->value() << std::endl;
	} else if(mResult->type() == JSON_BOOL) {
		std::cout << "Key: " << mResult->key() << std::endl;
		std::cout << "Value: " << dynamic_cast<JsonBool *> (mResult)->value() << std::endl;
	}
	
}

inline JsonDbActor::JsonDbActor(Stateplex::Dispatcher *dispatcher)
	: Object(this), Actor(dispatcher)
{ }

void JsonDbActor::getRootObject(const char *path, JsonObject *item)
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_GET, Stateplex::String::copy(allocator(), path), mSender, this, item, mCallbackFunction);
	queueMessage(rootObject);
}

void JsonDbActor::setJsonObject(const char *path, const char *parameter, JsonObject *item)
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_SET, Stateplex::String::copy(allocator(), path), Stateplex::String::copy(allocator(), parameter), mSender, this, item, mCallbackFunction);
	queueMessage(rootObject);
}

void JsonDbActor::addJson(const char *path, JsonItem *newItem, JsonObject *item)
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_ADD, Stateplex::String::copy(allocator(), path), newItem, mSender, this, item, mCallbackFunction);
	queueMessage(rootObject);
}

/*template<typename Sender> void JsonDbActor::setClient(Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message))
{
	//mSender = sender;
	//mCallbackFunction = callbackFunction;
}*/

#endif
