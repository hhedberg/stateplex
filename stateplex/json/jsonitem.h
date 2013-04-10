#ifndef JSONITEM_H
#define JSONITEM_H

#include "../core/list.h"
#include "../core/object.h"
#include <cstring>

enum JsonType {
		JSON_STRING,
		JSON_NUMBER,
		JSON_OBJECT,
		JSON_DATA
};

class JsonItem : public Stateplex::Object, public Stateplex::ListItem
{

public:
	JsonItem(Stateplex::Actor *owner);
	virtual ~JsonItem();
	virtual void traverse() = 0;
	virtual const char *key() = 0;
	virtual JsonType type() = 0;

protected:

	const char *mKey;
	JsonType mType;

private:
	
};

/*** Inline Implementation ***/

inline JsonItem::JsonItem(Stateplex::Actor *owner)
	: Object(owner)
{ }

inline JsonItem::~JsonItem()
{ }

#endif // JSONITEM_H
