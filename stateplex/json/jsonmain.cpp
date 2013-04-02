#include "jsonitem.h"
#include "jsonnumber.h"
#include "jsonobject.h"
#include "jsondata.h"

int main()
{
	Stateplex::List<JsonObject> mList;
	JsonObject mObjects("TARMO");
	JsonObject mJohn("REINO");

	JsonData<char> stringi("stringi", "kaalimato");
	int number = 17;
	JsonData<int> intti("intti", &number);


	for (int i = 0; i < 12; i++) {
		mObjects.add(new JsonNumber("seppo", i*3));
	}

	for (int y = 0; y < 7; y++) {
		mJohn.add(new JsonNumber("jukka", y * 43));	
	}

	mList.addTail(&mObjects);
	mList.addTail(&mJohn);

	for (Stateplex::ListIterator<JsonObject> iterator(&mList); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = iterator.current();
		
		std::cout << '"';
		std::cout << item->key();
		std::cout << '"';
		std::cout << ":";
		std::cout << " {" << std::endl;
		
		for(Stateplex::ListIterator<JsonItem> mIterator(item->getList()); mIterator.hasCurrent(); mIterator.subsequent()) {
			
			JsonNumber *number = reinterpret_cast<JsonNumber *>(mIterator.current());
			
			std::cout << "\t" << '"';
			std::cout << number->key();
			std::cout << '"';

			std::cout << " : ";
			std::cout << number->value();

			if(mIterator.hasSubsequent()) {
				std::cout << ",";
			}
			std::cout << std::endl;
		}		
		std::cout << "}";
			
			std::cout << std::endl;
	}

	std::cout << std::endl;

	std::cout << "JSONDATA_STRING: " << stringi.key() << " " << stringi.value() << std::endl;
	std::cout << "JSONDATA_INTTI: " << intti.key() << " " << *(intti.value());	
	
	for (Stateplex::ListIterator<JsonObject> iterator(&mList); iterator.hasCurrent(); iterator.subsequent()) {
			JsonObject *item = iterator.current();
			item->freeItems();
	}

  return 0;
}


