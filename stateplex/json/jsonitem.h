#ifndef JSONITEM_H
#define JSONITEM_H

#include "../core/list.h"

class JsonItem : public Stateplex::ListItem
{
public:
	JsonItem();
	virtual ~JsonItem();
	virtual void traverse() = 0;
	virtual Json *data() = 0;

protected:
	Json data;
private:
};


class Storage
{
private:
	
};

#endif // JSONITEM_H
