#include "jsonobject.h"
#include <iostream>
#include <cstdlib>
#include <ctype.h>

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

JsonItem *JsonObject::get(Stateplex::String *path)
{
	std::vector<std::string> tokens;
	JsonItem *test = NULL;
	JsonObject *test2 = NULL;
	

	std::string str(path->chars());

	tokenizepath(str, tokens);
	
	for (int i = 0; i < tokens.size(); i++) {
		test2 = dynamic_cast<JsonObject *>(test);
		if(test == NULL) {
			if (find(tokens[i].c_str())->type() == JSON_OBJECT) {
				test2 = dynamic_cast<JsonObject *> (find(tokens[i].c_str()));
				test = dynamic_cast<JsonItem *> (test2);
			} else {
				test = find(tokens[i].c_str());
			}
			
			//std::cout << "S" << test->key();
		} else {
			if(test2->find(tokens[i].c_str())->type() == JSON_OBJECT) {
				test2 = dynamic_cast<JsonObject *> (test2->find(tokens[i].c_str()));
				test = dynamic_cast<JsonItem *> (test2);
			} else {
				test = test2->find(tokens[i].c_str());
			}

			if(test2 == NULL) {
				std::cout << "NULL";
			}
			if(test2 != NULL) {
				//std::cout << " SE " << test2->key();
			}
			
			if(test != NULL) {
				//std::cout << " se " << test->key();
			}
			
		}
		
	}

	ref();
	return test;
}

JsonObject *JsonObject::set(Stateplex::String *path, Stateplex::String *parameter)
{
	JsonItem *test = get(path);
	
	std::vector<std::string> tokens;
	std::string str(path->chars());
	
	tokenizepath(str, tokens, "/");

	for(int i = 0; i < tokens.size(); i++) std::cout << "value: " <<  tokens[i] << std::endl;

	std::cout << get(path);
	std::cout << "seppo tokens " << tokens[0] << std::endl;

	

	if(test != NULL) {
		std::cout << "test object set print seppo" <<  std::endl;
		if(isInteger(&tokens[1])) {
			int value = std::atoi(tokens[1].c_str());
			std::cout << "seppo on taalla taas" << std::endl;
			//std::cout << "test object set print int " << test->key() << std::endl;
		}
	} else { std::cout << "seppo oli null" << std::endl; }

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

bool JsonObject::isInteger(const std::string *s)
{
	/*if(s->empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) {
		return false;
	}

	char *p;
	strtol(s.c_str(), &p, 10) ;

	return (*p == 0) ;*/
}

void JsonObject::ref()
{
	mRefcount++;
}

void JsonObject::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

