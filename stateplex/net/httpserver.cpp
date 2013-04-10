/*
 * Stateplex - A server-side actor model library.
 *
 * net/httpserver.cpp
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

#include "httpserver.h"
#include "httpconnection.h"

namespace Stateplex {

TcpConnection *HttpServer::instantiateTcpConnection(const TcpConnection::Embryo *embryo)
{
	return new HttpConnection(embryo->mTcpServer->actor(), this, embryo);
}

HttpRequest *HttpServer::instantiateHttpRequest(const HttpRequest::Embryo *embryo)
{
	/* TODO: take the path into account */
	return mRequestFactoryMethod.invoke(embryo);
}

}
