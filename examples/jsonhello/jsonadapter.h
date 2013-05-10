#ifndef JSONADAPTER_H
#define JSONADAPTER_H

#include "jsoninterface.h"
#include <stateplex/json/jsondbactor.h>

template<class T, class M>
class JsonAdapter : public JsonInterface<T,M>
{
public:
	JsonAdapter();
	~JsonAdapter();
	void set(const char *path, const char *parameter, JsonObject *item, T *sender, void(T::*function)(M *message));
	void get(const char *path, JsonObject *item, T *sender, void(T::*function)(M *message));
	void add(const char *path, JsonItem *newItem, JsonObject *item, T *sender, void(T::*function)(M *message));
	
	void plugin(JsonDbActor *source);
private:
	JsonDbActor *mSource;

protected:
	
};

/*** Inline implementation ***/

template<class T, class M>
inline void JsonAdapter<T,M>::set(const char *path, const char *parameter, JsonObject *item, T *sender, void(T::*function)(M *message))
{
	mSource->setJsonObject(path, parameter, item, sender, function);
}

template<class T, class M>
inline void JsonAdapter<T,M>::get(const char *path, JsonObject *item, T *sender, void(T::*function)(M *message))
{
	mSource->getRootObject(path, item, sender, function);
}

template<class T, class M>
inline void JsonAdapter<T,M>::add(const char *path, JsonItem *newItem, JsonObject *item, T *sender, void(T::*function)(M *message))
{
	mSource->addJson(path, newItem, item, sender, function);
}

template<class T, class M>
inline void JsonAdapter<T,M>::plugin(JsonDbActor *source)
{
	mSource = source;
}

template<class T, class M>
JsonAdapter<T,M>::JsonAdapter() 
{ 
	//ctor
}

template<class T, class M>
JsonAdapter<T,M>::~JsonAdapter()
{
	//dtor
}


#endif
