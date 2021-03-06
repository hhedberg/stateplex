/*
 * Stateplex - A server-side actor model library.
 *
 * net/httprequest.h
 *
 * (c) 2013 Henrik Hedberg <henrik.hedberg@innologies.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Authors: Henrik Hedberg
 */

#ifndef INCLUDED_STATEPLEX_HTTP_REQUEST_H
#define INCLUDED_STATEPLEX_HTTP_REQUEST_H

#include <sys/types.h>
#include <sys/socket.h>

#include "../core/object.h"
#include "../core/writebuffer.h"

namespace Stateplex {

class HttpServer;
class TcpConnection;
class IoSource;
class HttpConnection;

/**
 * HTTP request handler.
 * One HttpConnection may carry through multiple requests.
 */

class HttpRequest : public Object {
	friend class HttpConnection;

	HttpConnection *mHttpConnection;
	WriteBuffer mData;
	Size mDataLeft;

	int mStatusSent : 1;

protected:
	/**
	 * Receives an HTTP header belonging to the request.
	 */
	virtual bool receiveHeader(Buffer *name, Buffer *value) = 0;

	/**
	 * Receives an HTTP message body content belonging to the request.
	 */
	virtual bool receiveData(Buffer *data) = 0;

	/**
	 * Indicates that the full HTTP request is now received.
	 *
	 * The HttpServer does not hold pointer nor call any of the functions of the HttpRequest after this call.
	 * The HttpRequest must take care of freeing all of its resources by its own.
	 */
	virtual void receiveEnd() = 0;

	/**
	 * Indicates that the HTTP request is aborted, because the other end has closed the connection.
	 *
	 * The HttpServer does not hold pointer nor call any of the functions of the HttpRequest after this call.
	 * The HttpRequest must take care of freeing all of its resources by its own.
	 */
	virtual void receiveAbort() = 0;

public:
	class Embryo {
		HttpConnection *mHttpConnection;
		Buffer *mMethod;
		Buffer *mUri;
		Buffer *mVersion;

	public:
		Embryo(HttpConnection *httpConnection, Buffer *method, Buffer *uri, Buffer *version);

		HttpConnection *httpConnection() const;
		Buffer *method() const;
		Buffer *uri() const;
		Buffer *version() const;
	};

	HttpRequest(const Embryo *embryo);
	virtual ~HttpRequest();

	HttpConnection *httpConnection() const;

	void sendStatus(const String *status);
	void sendStatus(Buffer *status);
	void sendHeader(const String *name, const String *value);
	void sendHeader(Buffer *name, Buffer *value);
	void sendData(const String *data);
	void sendData(Buffer *data);
	void sendEnd();
};

class SimpleHttpRequest : public HttpRequest {
	String *mStatus;
	String *mBody;

protected:
	virtual bool receiveHeader(Buffer *name, Buffer *value);
	virtual bool receiveData(Buffer *data);
	virtual void receiveEnd();
	virtual void receiveAbort();

public:
	SimpleHttpRequest(const HttpRequest::Embryo *embryo, const char *status);
	SimpleHttpRequest(const HttpRequest::Embryo *embryo, const char *status, const char *body);
};


}

/*** Inline implementations ***/

#include "tcpconnection.h"
#include "httpconnection.h"

namespace Stateplex {

/**
 * Constructs a new request and associates it to the given HttpConnection.
 */

inline HttpRequest::HttpRequest(const Embryo *embryo)
	: Object(embryo->httpConnection()->actor()), mHttpConnection(embryo->httpConnection()), mData(actor()), mDataLeft(0), mStatusSent(false)
{ }

/**
 * Destroys the request.
 */

inline HttpRequest::~HttpRequest()
{
	/* TODO: mTcpConnection->unsetHandler(); */
}

/**
 * Returns the HttpConnection associated with the request.
 */

inline HttpConnection *HttpRequest::httpConnection() const
{
	return mHttpConnection;
}

inline HttpRequest::Embryo::Embryo(HttpConnection *httpConnection, Buffer *method, Buffer *uri, Buffer *version)
	: mHttpConnection(httpConnection), mMethod(method), mUri(uri), mVersion(version)
{ }

inline HttpConnection *HttpRequest::Embryo::httpConnection() const
{
	return mHttpConnection;
}

inline Buffer *HttpRequest::Embryo::method() const
{
	return mMethod;
}

inline Buffer *HttpRequest::Embryo::uri() const
{
	return mUri;
}

inline Buffer *HttpRequest::Embryo::version() const
{
	return mVersion;
}

}

#endif
