/*
 * Stateplex - A server-side actor model library.
 *
 * net/hbdpserver.h
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

#ifndef INCLUDED_STATEPLEX_HBDP_SERVER_H
#define INCLUDED_STATEPLEX_HBDP_SERVER_H

#include "../core/factorymethod.h"
#include "../core/list.h"
#include "hbdpconnection.h"
#include "httprequest.h"

namespace Stateplex {

class HttpServer;

/**
 * The main class to implement a HTTP Bidirectional Protocol server.
 */
class HbdpServer {
	HttpServer *mHttpServer;
	FactoryMethod<HbdpConnection, HbdpConnection::Embryo> mConnectionFactoryMethod;
	List<HbdpConnection> mConnections;

	HttpRequest *instantiateHttpRequest(const HttpRequest::Embryo *embryo);

public:
	HbdpServer(HttpServer *httpServer, const char *path);

	template<typename T> void setConnectionFactoryMethod(T *object, HbdpConnection *(T::*function)(const HbdpConnection::Embryo *embryo));
};

}

/*** Inline implementations ***/

#include "httpserver.h"

namespace Stateplex {

inline HbdpServer::HbdpServer(HttpServer* httpServer, const char *path)
	: mHttpServer(httpServer)
{
	mHttpServer->setRequestFactoryMethod(path, this, &HbdpServer::instantiateHttpRequest);
}

template<typename T>
inline void HbdpServer::setConnectionFactoryMethod(T *object, HbdpConnection *(T::*function)(const HbdpConnection::Embryo *embryo))
{
	mConnectionFactoryMethod.set(object, function);
}

}

#endif
