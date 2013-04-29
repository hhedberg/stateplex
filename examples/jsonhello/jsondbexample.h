#ifndef JSONDBEXAMPLE_H
#define JSONDBEXAMPLE_H

#include <stateplex/json/jsonobject.h>
#include "jsondb.h"

class JsonDbExample : public JsonDb
{
public:
	JsonDbExample();
        virtual ~JsonDbExample();
	void initDb(JsonDbActor *jsonActor, JsonClient *client);
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

void JsonDbExample::initDb(JsonDbActor *jsonActor, JsonClient *client)
{
	root = new JsonObject(jsonActor, "Stateplex");
	root->add(new JsonObject(jsonActor, "Employee"));
	root->getObject("Employee")->add(new JsonObject(jsonActor, "Juha"));
	root->getObject("Employee")->add(new JsonObject(jsonActor, "Tapio"));

	root->getObject("Employee/Juha/")->add(new JsonNumber(jsonActor, "Age", 24));
	root->getObject("Employee/Tapio/")->add(new JsonDouble(jsonActor, "Age", 24.423));
	root->getObject("Employee/Tapio/")->add(new JsonString(jsonActor, "Mummo", "Inkeri"));
	root->getObject("Employee/Tapio/")->add(new JsonBool(jsonActor, "Testi", true));

	jsonActor->getRootObject("Employee/Juha/Age/", client, root, &JsonClient::showResult);
	jsonActor->setJsonObject("Employee/Juha/Age/", "14", client, root, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/Tapio/Age/", client, root, &JsonClient::showResult);
	jsonActor->getRootObject("Employee/Tapio/Testi/", client, root, &JsonClient::showResult);

	jsonActor->addJson("Employee/", new JsonObject(jsonActor, "Seppo"), client, root, &JsonClient::showResult);
	jsonActor->addJson("Employee/Seppo/", new JsonDouble(jsonActor, "Length", 172.43), client, root, &JsonClient::showResult);	
	
}

#endif // JSONDBEXAMPLE_H
