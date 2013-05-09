#ifndef JSONITEM_H
#define JSONITEM_H

#include "../core/list.h"
#include "../core/object.h"
#include "../core/string.h"
#include "../core/types.h"

enum JsonType {
		JSON_STRING,
		JSON_NUMBER,
		JSON_BOOL,
		JSON_DOUBLE,
		JSON_OBJECT,
		JSON_DATA
};

/**
 * @brief Super class for different types of json.
 * 
 * This class is inherited from Object and ListItem.
 * ListItems are used to store json objects/items to
 * list and they can be accessed from there with different
 * functions.
 */

class JsonItem : public Stateplex::Object, public Stateplex::ListItem
{

public:
	JsonItem(Stateplex::Actor *owner);
	virtual ~JsonItem();
	virtual void traverse() = 0;
	virtual void unref() = 0;
	virtual const char *key() = 0;
	virtual JsonType type() = 0;
	virtual Stateplex::Size32 refcount() = 0;

protected:
	char *mKey;
	virtual void ref() = 0;
	JsonType mType;
	Stateplex::Size32 mRefcount;

private:
	
};

/*** Inline Implementation ***/

/**
 * Constructor that takes the owner actor as
 * parameter and initializes.
 */

inline JsonItem::JsonItem(Stateplex::Actor *owner)
	: Object(owner)
{ }

/**
 * Default destructor for class JsonItem.
 */

inline JsonItem::~JsonItem()
{ }

#endif // JSONITEM_H
