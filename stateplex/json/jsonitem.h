#ifndef JSONITEM_H
#define JSONITEM_H

#include "../core/list.h"
#include <string>

class JsonItem : public Stateplex::ListItem
{
public:
	JsonItem();
	virtual ~JsonItem();
	virtual void traverse() = 0;
	virtual const char *key() = 0;

protected:
	const char *mKey;
private:
	
};

#endif // JSONITEM_H
