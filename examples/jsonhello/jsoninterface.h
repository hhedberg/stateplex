#ifndef JSONINTERFACE_H
#define JSONINTERFACE_H

template<class T, class M>
class JsonInterface
{
public:
	virtual void get(const char *path, JsonObject *item, T *sender, void(T::*function)(M *message) ) = 0;
	virtual void set(const char *path, const char *parameter, JsonObject *item, T *sender, void(T::*function)(M *message)) = 0;
	virtual void add(const char *path, JsonItem *newItem, JsonObject *item, T *sender, void(T::*function)(M *message)) = 0;
protected:
	
private:
};

#endif // JSONINTERFACE_H
