#ifndef JSONDB_H
#define JSONDB_H

#include <stateplex/json/jsondbactor.h>
#include "jsonclient.h"

class JsonDb
{
public:
	JsonDb();
        virtual ~JsonDb();
	virtual void initDb(JsonDbActor *jsonActor, JsonClient *client, JsonAdapter *adapter) =0;
protected:
	
private:
};

/*** Inline implementations ***/

inline JsonDb::JsonDb()
{ }

inline JsonDb::~JsonDb()
{ }

#endif // JSONDB_H
