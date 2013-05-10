#ifndef INCLUDED_JSON_DB_ACTOR_H
#define INCLUDED_JSON_DB_ACTOR_H

#include "../core/callbackmessage.h"
#include "../core/actor.h"
#include "../core/string.h"
#include "jsonobject.h"

 /**
* JsonMessageType enum is used to see which kind of operation
* is needed to be executed at different times.
*/

enum JsonMessageType {
	JSON_MESSAGE_TYPE_GET,
	JSON_MESSAGE_TYPE_SET,
	JSON_MESSAGE_TYPE_ADD
};

/**
 * @brief JsonDbActor handles json items.
 *
 * JsonDbActor can add/set/get jsonitems from/to assigned
 * jsonobject.
 */

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
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Stateplex::String *parameter, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		template<typename Sender> JsonMessage(JsonMessageType type, Stateplex::String *path, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message));
		void result() const;

	protected:
		void handle(Actor *sender, JsonDbActor *receiver);
		
	};
	
	template<typename Sender> void getRootObject(const char *path, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message)); 
	template<typename Sender> void setJsonObject(const char *path, const char *parameter, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message)); 
	template<typename Sender> void addJson(const char *path, JsonItem *newItem, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message)); 
};

/*** Inline implementations ***/

#include <iostream>

/**
 * Constructor that creates a new JsonMessage
 * which is used for adding a new json item to the
 * target object. 
 * 
 * @param type			type of message.
 * @param *path			path to where to add the new json item.
 * @param *newItem		new json item to add.
 * @param *sender		how is the sender of the JsonMessage.
 * @param *receiver		how is the receiver of the JsonMessage.
 * @param *item			target object where new json item will be added.
 * @param *callbackFunction	function to call when new json item has been added.
 * 
 * @see				JsonMessageType
 */

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, JsonItem *newItem, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type),mPath(path), mNew(newItem)
{ }

/**
 * Constructor that creates a new JsonMessage
 * which is used for setting a value of json item to a new value.
 * 
 * @param type			type of message.
 * @param *path			path to where to set a new value.
 * @param *parameter		new value for the target json item.
 * @param *sender		how is the sender of the JsonMessage.
 * @param *receiver		how is the receiver of the JsonMessage.
 * @param *item			target object where new json item will be added.
 * @param *callbackFunction	function to call when new json item has been added.
 * 
 * @see				JsonMessageType
 */

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, Stateplex::String *parameter, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type), mPath(path), mParameter(parameter)
{ }

/**
 * Constructor that creates a new JsonMessage
 * which is used for getting a value of target json item.
 * 
 * @param type			type of message.
 * @param *path			path to where to get the value.
 * @param *sender		how is the sender of the JsonMessage.
 * @param *receiver		how is the receiver of the JsonMessage.
 * @param *item			target object where new json item will be added.
 * @param *callbackFunction	function to call when new json item has been added.
 * 
 * @see				JsonMessageType
 */

template<typename Sender> JsonDbActor::JsonMessage::JsonMessage(JsonMessageType type, Stateplex::String *path, Sender *sender, JsonDbActor *receiver, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
	: Stateplex::CallbackMessage<JsonDbActor, JsonMessage>(sender, receiver, callbackFunction), mResult(item), mType(type), mPath(path)
{ }

/**
 * Handles JsonMessage based on message type and
 * notifies sender.
 * 
 * @param *sender		target sender to notify.
 * @param *receiver		target receiver to notify.
 */

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

/**
 * Prints out the result from last jsonmessage.
 * or in other words prints the key and value if possible.
 */

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

/**
 * Constructor that takes dispatcher address as parameter
 * and assigns it to handle this.
 */

inline JsonDbActor::JsonDbActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{ }

/**
 * Adds a jsonmessage to a queue which will then
 * get the wanted information about the jsonitem.
 * 
 * @param *path			path where to get the json item.
 * @param *item			target object where the json item is located.
 * @param *sender		sender of the JsonMessage.
 * @param *callbackFunction	function to callback after jsonmessage has been handled.
 */

template<typename Sender> void JsonDbActor::getRootObject(const char *path, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message)) 
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_GET, Stateplex::String::copy(allocator(), path), sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

/**
 * Adds a jsonmessage to a queue which will then
 * set the wanted information to a specific json item.
 * 
 * @param *path			path where to set the information to json item.
 * @param *parameter		wanted information to set into the json item.
 * @param *item			target object where the json item is located.
 * @param *sender		sender of the JsonMessage.
 * @param *callbackFunction	function to callback after jsonmessage has been handled.
 */

template<typename Sender> void JsonDbActor::setJsonObject(const char *path, const char *parameter, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message))
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_SET, Stateplex::String::copy(allocator(), path), Stateplex::String::copy(allocator(), parameter), sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

/**
 * Adds a jsonmessage to a queue which will then
 * add a new json item to specified jsonobject.
 * 
 * @param *path			path where to set the information to json item.
 * @param *newItem		new item to add.
 * @param *item			target object where the json item is located.
 * @param *sender		sender of the JsonMessage.
 * @param *callbackFunction	function to callback after jsonmessage has been handled.
 */

template<typename Sender> void JsonDbActor::addJson(const char *path, JsonItem *newItem, JsonObject *item, Sender *sender, void (Sender::*callbackFunction)(JsonMessage *message)) 
{
	JsonMessage *rootObject = new JsonMessage(JSON_MESSAGE_TYPE_ADD, Stateplex::String::copy(allocator(), path), newItem, sender, this, item, callbackFunction);
	queueMessage(rootObject);
}

#endif
