#include "jsonobject.h"

/**
 * Constructor that assigns the key and owner
 * of the jsonobject object.
 *
 * @param *owner	pointer to the owner actor.
 * @param *key		wanted value for the key.
 */
 
JsonObject::JsonObject(Stateplex::Actor *owner, const char *key)
	: JsonItem(owner)
{
    	mKey = const_cast<char *> (key);
	mItems = new Stateplex::List<JsonItem>;
	mType = JSON_OBJECT;
}

/**
 * Destructor that deletes objects included in
 * JsonObject object.
 */
 
JsonObject::~JsonObject()
{
    	freeItems();
	delete mItems;
}

/**
 * Adds given JsonItem to this JsonObjects list.
 *
 * @param *item		JsonItem to add.
 * @see			Stateplex::List.
 */

void JsonObject::add(JsonItem *item)
{
	mItems->addTail(item);
}

/**
 * Finds JsonObject based on the path given as parameter and adds given
 * JsonItem to it's list.
 *
 * @param *path		path to be used in the search of the target object.
 * @param *item		JsonItem to add.
 * @see			Stateplex::List.
 */
 
void JsonObject::add(Stateplex::String *path, JsonItem *item)
{
	JsonObject *tmp = NULL;
	tmp = dynamic_cast<JsonObject *>(get(path));
	tmp->add(item);
}

/**
 * Goes through the JsonItems included in the JsonObject object
 * and asks them to print their information.
 */

void JsonObject::traverse()
{
	int count = 0;
	std::cout << '"' <<  key() << '"' << " : " << "{" << std::endl;
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		if(iterator.hasSubsequent()) {
			for(int i = 0; i < count; i++) {
				std::cout << "\t";	
			}
		}
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		item->traverse();
	}

	std::cout << "}" << std::endl;
}

/**
 * Returns the JsonItems in this JsonObject.
 *
 * @return		List that includes the JsonItems in this JsonObject.
 * @see			Stateplex::List.
 */
 
Stateplex::List<JsonItem> *JsonObject::getList()
{
	return mItems;
}

/**
 * Returns the key of the JsonObject object.
 *
 * @return		key of JsonObject object.
 */
 
const char *JsonObject::key()
{
	return mKey;
}

/**
 * Returns the type of the JsonObject object.
 * Can be used for comparison to verify that
 * an object is actually of JsonObject type.
 *
 * @return		type of the JsonObject object.
 */
 
JsonType JsonObject::type()
{
	return mType;
}

/**
 * Sets given parameter as key.
 *
 * @param *str		parameter key to be set as key.
 * @see			Stateplex::String.
 */
 
void JsonObject::setKey(Stateplex::String *str)
{
	mKey = str->chars();
}

/**
 * Frees the memory used by JsonItems included in the JsonObject object.
 */
 
void JsonObject::freeItems()
{
	for (Stateplex::ListIterator<JsonItem> iterator(mItems); iterator.hasCurrent(); iterator.subsequent()) {
		JsonObject *item = reinterpret_cast<JsonObject *>(iterator.current());
		delete item;
	}

}

/**
 * Finds JsonItem based on the path given as parameter.
 *
 * @param *path		path to be used in the search of the item.
 * @return		JsonItem that was found, or NULL if not found.
 */
 
JsonItem *JsonObject::get(const char *path)
{
	JsonItem *item = get(Stateplex::String::copy(allocator(), path));
	return item;
}

/**
 * Finds JsonObject based on the path given as parameter.
 *
 * @param *path		path to be used in the search of the object.
 * @return		JsonObject that was found, or NULL if not found.
 */

JsonObject *JsonObject::getObject(const char *path)
{
	JsonObject *object = dynamic_cast<JsonObject *> (get(Stateplex::String::copy(allocator(), path)));
	return object;
	
}

/**
 * Finds JsonItem based on the path given as parameter and returns
 * it to the caller.
 *
 * @param *path		path to be used in the search of the item.
 * @return		JsonItem that was found, or NULL if not found.
 * @see			Stateplex::String.
 */
 
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

/**
 * Finds JsonItem based on the path given and updates the value
 * based on the given parameter value.
 *
 * @param *path		path where modification is to be made.
 * @param *parameter	value to be set.
 * @return		NULL if something went wrong.
 * @see 		Stateplex::String.
 */
 
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

		case JSON_BOOL:
			if(strcmp(parameter->chars(), "true") == 0) {
				dynamic_cast<JsonBool *>(test)->setValue(true);
			} else if(strcmp(parameter->chars(), "false") == 0) {
				dynamic_cast<JsonBool *>(test)->setValue(false);
			}

			break;

		case JSON_DOUBLE:
			if(isDouble(parameter)) {
				dynamic_cast<JsonDouble *>(test)->setValue(toDouble(parameter));
			}
				
			break;
		
		default:
			return NULL;
			break;
		
	}
}

/**
 * Finds JsonItem based on the key value given as parameter.
 *
 * @param *target	key value that is used in the search.
 * @return		JsonItem that was found, or NULL if not found.
 */
 
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

/**
 * Finds object of type JsonObject based on the key given.
 *
 * @param *target	key value that is used in the search.
 * @return		JsonObject that was found, 0 if not found.
 */
 
JsonObject *JsonObject::findObject(const char *target)
{
	JsonItem *item = find(target);

	if(item->type() != JSON_OBJECT) {
		return 0;
	}

	return dynamic_cast<JsonObject *> (item);
}

/**
 * Splits given std::string in to tokens based on the delimiter, which
 * default value is '/'. Tokens are stored in the given std::vector.
 * 
 * @param &str		std:string to split into tokens.		
 * @param &tokens	std::vector where tokens are to be stored.
 * @param delimiter	character to be used as a delimiter.
 */

void JsonObject::tokenizepath(const std::string& str, std::vector<std::string>& tokens, const char delimiter)
{
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
}

/**
 * Checks if given parameter has an integer value.
 *
 * @return		true if parameter is integer, otherwise false.
 * @see			Stateplex::String.
 */
 
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

/**
 * Changes given parameter to an integer value and returns
 * it to the caller.
 *
 * @return		given parameter as integer value.
 * @see			Stateplex::String.
 */
 
int JsonObject::toInteger(Stateplex::String *str)
{
	int value;
	std::stringstream char_str; 
	
	char_str << str->chars();
	char_str >> value;
	
	return value;
}

bool JsonObject::isDouble(Stateplex::String *str)
{
	bool isDouble = false;
	const char *s = str->chars();

	if(strtod(s, NULL)) {
		isDouble = true;
	}

	return isDouble;
}

double JsonObject::toDouble(Stateplex::String *str)
{
	double value = strtod(str->chars(), NULL);

	return value;
}


/**
 * Increases the reference count by one.
 * Reference count is used to monitor/handle the reading
 * and writing of the object.
 */
 
void JsonObject::ref()
{
	mRefcount++;
}

/**
 * Decreases the reference count by one.
 * Reference count is used to monitor/handle the reading
 * and writing of the object.
 */
 
void JsonObject::unref()
{
	if (mRefcount > 0) {
		mRefcount--;
	}
}

/**
 * Returns the reference count.
 *
 * @return	reference count as Size32.
 * @see 	Stateplex::Size32.
 */
 
Stateplex::Size32 JsonObject::refcount() 
{
	return mRefcount;
}
