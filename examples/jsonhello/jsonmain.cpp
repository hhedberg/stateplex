#include <stateplex/json/jsonitem.h>
#include <stateplex/json/jsonnumber.h>
#include <stateplex/json/jsonstring.h>
#include <stateplex/json/jsonobject.h>
#include <stateplex/json/jsondouble.h>
#include <stateplex/json/jsonbool.h>
#include <stateplex/core/dispatcher.h>
#include <stateplex/json/jsondbactor.h>
#include "jsonclient.h"
#include "jsonadapter.h"
#include "jsondbexample.h"

int main()
{
	Stateplex::Dispatcher dispatcher;
	JsonDbActor jsonactor(&dispatcher);
	JsonClient client(&dispatcher);
	JsonAdapter<JsonClient, JsonDbActor::JsonMessage> adapter;
	JsonDbExample json;
	
	json.initDb(&jsonactor, &client, &adapter);
	
	dispatcher.run();

	return 0;
}


