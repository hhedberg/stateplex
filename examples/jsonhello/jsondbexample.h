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
	root->getObject("Employee")->add(new JsonObject(jsonActor, "Michael"));
	root->getObject("Employee")->add(new JsonObject(jsonActor, "John"));

	root->getObject("Employee/Michael/")->add(new JsonNumber(jsonActor, "Age", 24));
	root->getObject("Employee/John/")->add(new JsonDouble(jsonActor, "Age", 24.423));
	root->getObject("Employee/John/")->add(new JsonString(jsonActor, "Elder", "Person"));
	root->getObject("Employee/John/")->add(new JsonBool(jsonActor, "BoolTest", true));

	/* This way of using get/set/add which is commented out does work but is not recommended!!!

	jsonActor->getRootObject("Employee/Michael/Age/", root, client,  &JsonClient::showResult);
	
	jsonActor->setJsonObject("Employee/Michael/Age/", "54", root, client, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/John/Age/", root, client, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/John/Testi/", root, client, &JsonClient::showResult);

	jsonActor->addJson("Employee/", new JsonObject(jsonActor, "Stanley"), root, client, &JsonClient::showResult);
	jsonActor->addJson("Employee/Stanley/", new JsonDouble(jsonActor, "Length", 172.43), root, client, &JsonClient::showResult);*/
	
	client->get("Employee/Michael/Age/", root, &JsonClient::showResult);
	client->set("Employee/Michael/Age/", "54", root, &JsonClient::showResult);
	client->get("Employee/John/Age/", root, &JsonClient::showResult);
	client->get("Employee/John/BoolTest/", root, &JsonClient::showResult);
	
	client->add("Employee/", new JsonObject(jsonActor, "Stanley"), root, &JsonClient::showResult);
	client->add("Employee/Stanley/", new JsonDouble(jsonActor, "Length", 172.43), root, &JsonClient::showResult);
	
	client->get("Employee/Stanley/", root, &JsonClient::showResult);
	client->set("Employee/Michael/Age/", "14", root, &JsonClient::showResult);
	client->add("Employee/Stanley/", new JsonString(jsonActor, "Beverage", "Milk"), root, &JsonClient::showResult);
	client->set("Employee/Stanley/Beverage/", "Water", root, &JsonClient::showResult); 
	client->set("Employee/Michael/Age/", "12", root, &JsonClient::showResult);
	client->get("Employee/John/Age/", root, &JsonClient::showResult);
	
}

#endif // JSONDBEXAMPLE_H
