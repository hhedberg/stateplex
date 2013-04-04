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

protected:
private:
};


class Storage
{
private:
	
};

#endif // JSONITEM_H
