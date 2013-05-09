#ifndef JSONDBEXAMPLE_H
#define JSONDBEXAMPLE_H

#include <stateplex/json/jsonobject.h>
#include "jsonadapter.h"
#include "jsondb.h"

class JsonDbExample : public JsonDb
{
public:
	JsonDbExample();
        virtual ~JsonDbExample();
	void initDb(JsonDbActor *jsonActor, JsonClient *client, JsonAdapter<JsonClient, JsonDbActor::JsonMessage> *adapter);
protected:
	
private:
	JsonObject *root;
};

/*** Inline Implementation ***/

inline JsonDbExample::JsonDbExample()
{ 
	
}

inline JsonDbExample::~JsonDbExample()
{ 
	if (root) {		
		delete root;
	}
}

void JsonDbExample::initDb(JsonDbActor *jsonActor, JsonClient *client, JsonAdapter<JsonClient, JsonDbActor::JsonMessage> *adapter)
{
	adapter->plugin(jsonActor);
	client->plugin(adapter);
	
	root = new JsonObject(jsonActor, "Stateplex");
	root->add(new JsonObject(jsonActor, "Employee"));
	root->getObject("Employee")->add(new JsonObject(jsonActor, "Juha"));
	root->getObject("Employee")->add(new JsonObject(jsonActor, "Tapio"));

	root->getObject("Employee/Juha/")->add(new JsonNumber(jsonActor, "Age", 24));
	root->getObject("Employee/Tapio/")->add(new JsonDouble(jsonActor, "Age", 24.423));
	root->getObject("Employee/Tapio/")->add(new JsonString(jsonActor, "Mummo", "Inkeri"));
	root->getObject("Employee/Tapio/")->add(new JsonBool(jsonActor, "Testi", true));

	jsonActor->getRootObject("Employee/Juha/Age/", root, client,  &JsonClient::showResult);
	
	jsonActor->setJsonObject("Employee/Juha/Age/", "54", root, client, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/Tapio/Age/", root, client, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/Tapio/Testi/", root, client, &JsonClient::showResult);

	jsonActor->addJson("Employee/", new JsonObject(jsonActor, "Seppo"), root, client, &JsonClient::showResult);
	jsonActor->addJson("Employee/Seppo/", new JsonDouble(jsonActor, "Length", 172.43), root, client, &JsonClient::showResult);
	
	client->get("Employee/Seppo/", root, &JsonClient::showResult);
	client->set("Employee/Juha/Age/", "14", root, &JsonClient::showResult);
	client->add("Employee/Seppo/", new JsonString(jsonActor, "Beer", "Riedenberger"), root, &JsonClient::showResult);
	client->set("Employee/Seppo/Beer/", "Riedenburger", root, &JsonClient::showResult); 
	
}

#endif // JSONDBEXAMPLE_H
