#ifndef JSONINTERFACE_H
#define JSONINTERFACE_H

class JsonInterface
{
public:
	virtual void get(const char *path) =0;
	virtual void set(const char *path, const char *value) =0;
protected:
	
private:
};

#endif // JSONDB_H
