/*
 * Stateplex - A server-side actor model library.
 *
 * examples/echohbdpserver/echohbdpserver.cpp
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

#include <sys/socket.h>
#include <netinet/in.h>

#include <stateplex/core/dispatcher.h>
#include <stateplex/net/tcpserver.h>
#include <stateplex/net/httpserver.h>
#include <stateplex/net/httprequest.h>
#include <stateplex/net/hbdpserver.h>
#include <stateplex/net/hbdpconnection.h>
#include <stateplex/core/identityfilter.h>

#include "echohbdpserver.h"

int main(void)
{
	Stateplex::Dispatcher dispatcher;
	EchoActor actor(&dispatcher);
	dispatcher.run();

	return 0;
}


EchoActor::EchoActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;
	Stateplex::TcpServer *tcpServer = new Stateplex::TcpServer(this, reinterpret_cast<sockaddr *>(&address), sizeof(address));
	Stateplex::HttpServer *httpServer = new Stateplex::HttpServer(tcpServer);
	mHbdpServer = new Stateplex::HbdpServer(httpServer, "/");
	mHbdpServer->setConnectionFactoryMethod(this, &EchoActor::instantiateHbdpConnection);
}

Stateplex::HbdpConnection *EchoActor::instantiateHbdpConnection(const Stateplex::HbdpConnection::Embryo *embryo)
{
	Stateplex::HbdpConnection *hbdpConnection = new Stateplex::HbdpConnection(this, embryo);
	Stateplex::IdentityFilter *identityFilter = new Stateplex::IdentityFilter(this, hbdpConnection);
	hbdpConnection->setReceiver(identityFilter);

	return hbdpConnection;
}
