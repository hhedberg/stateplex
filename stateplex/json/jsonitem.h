#ifndef JSONITEM_H
#define JSONITEM_H

#include "../core/list.h"
#include <cstring>

enum JsonType {
		JSON_STRING,
		JSON_NUMBER,
		JSON_OBJECT,
		JSON_DATA
};

class JsonItem : public Stateplex::ListItem
{

public:
	JsonItem();
	virtual ~JsonItem();
	virtual void traverse() = 0;
	virtual const char *key() = 0;
	virtual JsonType type() = 0;

protected:

	const char *mKey;
	//const char *mType;
	JsonType mType;

private:
	
};

#endif // JSONITEM_H
