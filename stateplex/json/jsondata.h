#ifndef JSONDATA_H
#define JSONDATA_H

#include "jsonitem.h"
#include <string>
#include <iostream>

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
protected:
private:
	//char *mKey;
	T *mValue;
};

template<typename T>
JsonData<T>::JsonData(Stateplex::Actor *owner, const char *key, T *value)
	: JsonItem(owner)
{ 
	mKey = key;
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

template<typename T>
JsonData<T>::~JsonData()
{
        //dtor
}

#endif // JSONDATA_H
