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


