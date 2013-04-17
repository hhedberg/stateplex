#include "jsonobject.h"
#include <iostream>

JsonObject::JsonObject(Stateplex::Actor *owner, const char *key)
	: JsonItem(owner)
{
    	mKey = key;
	mItems = new Stateplex::List<JsonItem>;
	mType = JSON_OBJECT;
}

JsonObject::~JsonObject()
{
    	freeItems();
	delete mItems;
}

void JsonObject::add(JsonItem *item)
{
	mItems->addTail(item);
}

void JsonObject::traverse()
{
	int count = 0;
	std::cout << '"' <<  key() << '"' << " : " << "{" << std::endl;
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(iterator.hasSubsequent()) {
			for(int i = 0; i < count; i++) {
				std::cout < "\t";	
			}
		}
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		item->traverse();
	}

	std::cout << "}" << std::endl;

}

Stateplex::List<JsonItem> *JsonObject::getList()
{
	return mItems;
}

const char *JsonObject::key()
{
	return mKey;
}

JsonType JsonObject::type()
{
	return mType;
}

void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		delete item;
	}

}

JsonObject *JsonObject::get(Stateplex::String *path)
{
		//IF(JSONMESSAGETYPEGET)
	//mResult->findObject(path);
	//PARSE STRING, SEPARATOR /
	//JsonObject *test = dynamic_cast<JsonObject *> (mResult);
	//ELSE
	//PARSE STRING, SEPARATOR / , 
	//mResult->set(path, parameter);
	//JsonObject *test = dynamic_cast<JsonObject *> (mResult);
	//invokeCallBack(this);
	std::vector<std::string> tokens;
	JsonObject *test = NULL;

	std::string str(path->chars());

	tokenizepath(str, tokens);

	for (int i = 0; i < tokens.size(); i++) {
		if(test == NULL) {
			test = findObject(tokens[i].c_str());
		} else {
			test = test->findObject(tokens[i].c_str());
		}
	}
	std::cout << std::endl;
	std::cout << "test object get print" << std::endl;
	return test;
}

JsonObject *JsonObject::set(Stateplex::String *path)
{
	std::cout << "test object set print" << std::endl;

	/*
	* Plan:
	* JsonObject *test = get(path);
	* tokenizepath(str, tokens, ',');
	* for example if(str castToint == true)
	* dynamic_cast<JsonNumber *> (test)->setValue(str);
 	* !!! REF COUNT ??? COPY OF THE JSONOBJECT BEFORE SETTING VALUE ???
	* GETFUNCTION( CONST JSONOBJECT ???? )
	*/

}

JsonItem *JsonObject::find(const char *target)
{
	JsonItem *item;
	
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(strcmp(iterator.current()->key(), target) == 0) {
			item = iterator.current();
			break;
		}
	}

	return item;
}

JsonObject *JsonObject::findObject(const char *target)
{
	JsonItem *item = find(target);

	if(item->type() != JSON_OBJECT) {
		return 0;
	}

	return dynamic_cast<JsonObject *> (item);
}



void JsonObject::tokenizepath(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

