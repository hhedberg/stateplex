#ifndef JSONDATA_H
#define JSONDATA_H

#include "jsonitem.h"
#include <string>
#include <iostream>
#include <sstream>

template<typename T>
class JsonData : public JsonItem
{
public:
	JsonData(Stateplex::Actor *owner, const char *key, T *value);
    	virtual ~JsonData();
	void traverse();
	const char *key();
	JsonType type();
	const T *value();
	void setValue(T *value);
	bool isInteger(Stateplex::String *str);
	int toInteger(Stateplex::String *str);
protected:
private:
	T *mValue;
};

template<typename T>
JsonData<T>::JsonData(Stateplex::Actor *owner, const char *key, T *value)
	: JsonItem(owner)
{ 
	mKey = const_cast<char *> (key);
	mValue = value;
	mType = JSON_DATA;
}

template<typename T>
void JsonData<T>::traverse()
{
	std::cout << "\t" << '"' << mKey << '"' << " : " << mValue << std::endl;
}

template<typename T>
const T *JsonData<T>::value()
{
	return mValue;
}

template<typename T>
const char *JsonData<T>::key()
{
	return mKey;
}

template<typename T>
JsonType JsonData<T>::type()
{
	return mType;
}
/*
template<typename T>
void JsonData<T>::setValue(T *value)
{
	*mValue = *value;
}*/

template<typename T>
bool JsonData<T>::isInteger(Stateplex::String *str)
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

template<typename T>
int JsonData<T>::toInteger(Stateplex::String *str)
{
	const char *s = str->chars();
	int value;
	std::stringstream char_str; 
	
	char_str << s;
	char_str >> value;
	
	return value;
}



template<typename T>
JsonData<T>::~JsonData()
{
        //dtor
}

#endif // JSONDATA_H
