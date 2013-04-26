#ifndef JSONADAPTER_H
#define JSONADAPTER_H

#include "jsoninterface.h"
#include "jsondbactor.h


class JsonAdapter : public JsonInterface, private jsondbActor
{
public:
	set(const char *path, const char *value);
	get(const char *path);
private:

protected:
	
}




/*** Inline implementation ***/

inline JsonAdapter::set(const char *path, const char *value, Sender *sender, JsonObject *item, void (Sender::*callbackFunction)(JsonMessage *message))
{
	//setJsonObject(path, value, sender, item, Sender::callbackFunction);
	
}
