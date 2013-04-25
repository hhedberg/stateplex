#include <stateplex/json/jsonitem.h>
#include <stateplex/json/jsonnumber.h>
#include <stateplex/json/jsonstring.h>
#include <stateplex/json/jsonobject.h>
#include <stateplex/json/jsondata.h>
#include <stateplex/core/dispatcher.h>
#include <stateplex/json/jsondbactor.h>
#include "jsonclient.h"
#include "jsondbexample.h"

int main()
{
	Stateplex::Dispatcher dispatcher;
	JsonDbActor jsonactor(&dispatcher);
	JsonClient client(&dispatcher);
	JsonDbExample jsonDbExample;
	
	jsonDbExample.initDb(&jsonactor, &client);
	
	
	/*Stateplex::List<JsonObject> mList;
	JsonObject mFirst(&jsonactor,"mFirst");
	JsonObject mSecond(&jsonactor,"mSecond");
	JsonObject company(&jsonactor,"Stateplex");

	company.add(new JsonObject(&jsonactor,"Employee"));
	company.findObject("Employee");
	company.findObject("Employee")->add(new JsonObject(&jsonactor,"Juha"));
	company.findObject("Employee")->add(new JsonObject(&jsonactor,"Tapio"));

	company.findObject("Employee")->findObject("Juha")->add(new JsonNumber(&jsonactor,"Age", 24));
	company.findObject("Employee")->findObject("Tapio")->add(new JsonNumber(&jsonactor,"Age", 24));

	jsonactor.getRootObject("Employee/Juha/Age/", &client, &company, &JsonClient::showResult);
	//jsonactor.setJsonObject("Employee/Juha/Age/", "14", &client, &company, &JsonClient::showResult);

	JsonData<const char> jsonChar(&jsonactor, "key", "value");
	int number = 17;
	JsonData<int> integer(&jsonactor, "key", &number);


	for (int i = 0; i < 12; i++) {
		mFirst.add(new JsonNumber(&jsonactor, "key", i*3));
	}

	for (int y = 0; y < 7; y++) {
		mSecond.add(new JsonNumber(&jsonactor, "key", y * 43));
		mSecond.add(new JsonString(&jsonactor, "strKey", "strTest"));	
	}

	mList.addTail(&mFirst);
	mList.addTail(&mSecond);

	for (Stateplex::ListIterator<JsonObject> iterator(&mList); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = iterator.current();
		
		std::cout << '"';
		std::cout << item->key();
		std::cout << '"';
		std::cout << ":";
		std::cout << " {" << std::endl;
		
		for(Stateplex::ListIterator<JsonItem> mIterator(item->getList()); mIterator.hasCurrent(); mIterator.subsequent()) {
			
			//JsonNumber *number = reinterpret_cast<JsonNumber *>(mIterator.current());
			
			// NOTE: probably not the best solution
			JsonNumber *number = dynamic_cast<JsonNumber *>(mIterator.current());
			
			if (number) {
				std::cout << "\t" << '"';
				std::cout << number->key();
				std::cout << '"';

				std::cout << " : ";
				std::cout << number->value();

				if(mIterator.hasSubsequent()) {
					std::cout << ",";
				}
			}
			else {
				JsonString *string = dynamic_cast<JsonString *>(mIterator.current());
				
				if (string) {
					std::cout << "\t" << '"';
					std::cout << string->key();
					std::cout << '"';

					std::cout << " : ";
					std::cout << string->value();

					if(mIterator.hasSubsequent()) {
						std::cout << ",";
					}
				}
			}
			std::cout << std::endl;
		}		
		std::cout << "}";
			
		std::cout << std::endl;
	}


	std::cout << std::endl;

	std::cout << "JSONDATA_CHAR: " << jsonChar.key() << " " << jsonChar.value() << std::endl;
	std::cout << "JSONDATA_INT: " << integer.key() << " " << *(integer.value()) << std::endl;

	std::cout << "JSON SEARCH: " << mSecond.find("strKey")->key() << std::endl;
	
	company.traverse(); */

	dispatcher.run();

  return 0;
}


