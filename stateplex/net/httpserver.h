/*
 * Stateplex - A server-side actor model library.
 *
 * net/httpserver.h
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

#ifndef INCLUDED_STATEPLEX_HTTP_SERVER_H
#define INCLUDED_STATEPLEX_HTTP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>

#include "../core/factorymethod.h"
#include "tcpconnection.h"
#include "httprequest.h"

namespace Stateplex {

class TcpServer;
class HttpConnection;

/**
 * The main class to implement a HTTP server.
 */
class HttpServer {
	friend class HttpConnection;

	TcpServer *mTcpServer;
	FactoryMethod<HttpRequest, HttpRequest::Embryo> mRequestFactoryMethod; /* TODO: take the path into account */

	TcpConnection *instantiateTcpConnection(const TcpConnection::Embryo *embryo);
	HttpRequest *instantiateHttpRequest(const HttpRequest::Embryo *embryo);

public:
	HttpServer(TcpServer *tcpServer);

	template<typename T> void setRequestFactoryMethod(const char *path, T *object, HttpRequest *(T::*function)(const HttpRequest::Embryo *embryo));
	bool unsetRequestFactoryMethod(const char *path);
};

}

/*** Inline implementations ***/

#include "tcpserver.h"

namespace Stateplex {

inline HttpServer::HttpServer(TcpServer* tcpServer)
	: mTcpServer(tcpServer)
{
	tcpServer->setTcpConnectionFactoryMethod(this, &HttpServer::instantiateTcpConnection);
}

template<typename T>
inline void HttpServer::setRequestFactoryMethod(const char* path, T* object, HttpRequest *(T::*function)(const HttpRequest::Embryo *embryo))
{
	/* TODO: handle path */
	mRequestFactoryMethod.set(object, function);
}

inline bool HttpServer::unsetRequestFactoryMethod(const char* path)
{
	/* TODO: implement */
	return false;
}

}

#endif
