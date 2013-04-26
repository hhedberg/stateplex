#include "jsonobject.h"
#include <iostream>
#include <cstdlib>
#include <ctype.h>

JsonObject::JsonObject(Stateplex::Actor *owner, const char *key)
	: JsonItem(owner)
{
    	mKey = const_cast<char *> (key);
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

void JsonObject::setKey(Stateplex::String *str)
{
	mKey = str->chars();
}

void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		delete item;
	}

}
JsonItem *JsonObject::get(const char *path)
{
	JsonItem *item = get(Stateplex::String::copy(allocator(), path));
	return item;
}

JsonObject *JsonObject::getObject(const char *path)
{
	JsonObject *object = dynamic_cast<JsonObject *> (get(Stateplex::String::copy(allocator(), path)));
	return object;
	
}

JsonItem *JsonObject::get(Stateplex::String *path)
{
	std::vector<std::string> tokens;
	JsonItem *test = NULL;
	JsonObject *test2 = NULL;
	const char *rightOne;

	std::string str(path->chars());
	
	tokenizepath(str, tokens);
	rightOne = tokens.back().c_str();
	
	for (int i = 0; i < tokens.size(); i++) {
		test2 = dynamic_cast<JsonObject *>(test);
		if(test == NULL) {
			if (find(tokens[i].c_str()) != NULL) {
				if (find(tokens[i].c_str())->type() == JSON_OBJECT) {
					test2 = dynamic_cast<JsonObject *> (find(tokens[i].c_str()));
					test = dynamic_cast<JsonItem *> (test2);
	
				} else {
					if(find(tokens[i].c_str()) != NULL) {	
						test = find(tokens[i].c_str());
					}	
				
				}
			
			}
			
		} else {
			if(test2->find(tokens[i].c_str()) != NULL) {
				if(test2->find(tokens[i].c_str())->type() == JSON_OBJECT) {
					test2 = dynamic_cast<JsonObject *> (test2->find(tokens[i].c_str()));
					test = dynamic_cast<JsonItem *> (test2);
				} else {
					if(test2->find(tokens[i].c_str()) != NULL) {
						test = test2->find(tokens[i].c_str());
					}
				}
			}
		}
		
	}

	ref();

	if(strcmp(rightOne, test->key()) != 0) {
		return NULL;
	}
	
	return test;
}

JsonObject *JsonObject::set(Stateplex::String *path, Stateplex::String *parameter)
{
	JsonItem *test = NULL;
	
	if(get(path) != NULL) {
		test = const_cast<JsonItem *> (get(path));
	} else {
		return NULL;
	}
	
	switch(test->type()) {
		
		case JSON_STRING:
			dynamic_cast<JsonString *>(test)->setValue(parameter);
		
			break;
		
		case JSON_NUMBER:
			if(isInteger(parameter)) {
				dynamic_cast<JsonNumber *>(test)->setValue(toInteger(parameter));
			}
			break;
		
		case JSON_OBJECT:
			dynamic_cast<JsonObject *>(test)->setKey(parameter);
		break;
		
		/*case JSON_DATA:
			//what is this madness
			if(isInteger(parameter)) {
				dynamic_cast<JsonData<int> *>(test)->setValue(toInteger(parameter));
			} else {
				dynamic_cast<JsonData<const char> *>(test)->setValue(parameter->chars());
			}
		
			break;
		*/
		default:
			return NULL;
			break;
		
	}


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
	JsonItem *item = NULL;
	
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

bool JsonObject::isInteger(Stateplex::String *str)
{
	bool isInteger = true;
	const char *s = str->chars();
	
	if(s[0] == '\0') {
		isInteger = false;
	}
	
	for(int i = 0; i < str->length(); i++) {
		if(!(isdigit(s[i]))) {
			isInteger = false;
		}
	}

	return isInteger;
}

int JsonObject::toInteger(Stateplex::String *str)
{
	const char *s = str->chars();
	int value;
	std::stringstream char_str; 
	
	char_str << s;
	char_str >> value;
	
	return value;
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

