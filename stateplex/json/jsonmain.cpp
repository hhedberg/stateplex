#include "jsonitem.h"
#include "jsonnumber.h"
#include "jsonstring.h"
#include "jsonobject.h"
#include "jsondata.h"

int main()
{
	Stateplex::List<JsonObject> mList;
	JsonObject mFirst("mFirst");
	JsonObject mSecond("mSecond");

	JsonData<char> jsonChar("key", "value");
	int number = 17;
	JsonData<int> integer("key", &number);


	for (int i = 0; i < 12; i++) {
		mFirst.add(new JsonNumber("key", i*3));
	}

	for (int y = 0; y < 7; y++) {
		mSecond.add(new JsonNumber("key", y * 43));	
	}

	mSecond.add(new JsonString("key", "test jsonString"));

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

	std::cout << "JSONDATA_CHAR: " << jsonChar.key() << " " << jsonChar.value() << std::endl;
	std::cout << "JSONDATA_INT: " << integer.key() << " " << *(integer.value()) << std::endl;	

  return 0;
}


