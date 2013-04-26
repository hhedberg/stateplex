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
	root->getObject("Employee/Tapio/")->add(new JsonNumber(jsonActor, "Age", 24));

	jsonActor->getRootObject("Employee/Juha/Age/", client, root, &JsonClient::showResult);
	jsonActor->setJsonObject("Employee/Juha/Age/", "14", client, root, &JsonClient::showResult);
	
}

#endif // JSONDBEXAMPLE_H
