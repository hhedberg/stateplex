#ifndef JSONSTRING_H
#define JSONSTRING_H

#include "jsonitem.h"
#include <string>
#include <iostream>

class JsonString : public JsonItem
{
public:
	JsonString();
	JsonString(std::string value);
        virtual ~JsonString();
	void traverse();
protected:
private:
	std::string mString;
};

#endif // JSONSTRING_H
