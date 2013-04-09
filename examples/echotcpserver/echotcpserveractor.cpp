/*
 * Stateplex - A server-side actor model library.
 *
 * examples/echohttpserver/echohttpserver.cpp
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
#include <stateplex/net/tcpconnection.h>


#include "echotcpserveractor.h"

int main(void)
{
	Stateplex::Dispatcher dispatcher;
	EchoTcpServerActor actor(&dispatcher);
	dispatcher.run();

	return 0;
}

EchoTcpServerActor::EchoTcpServerActor(Stateplex::Dispatcher *dispatcher)
	: Actor(dispatcher)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(12345);
	address.sin_addr.s_addr = INADDR_ANY;
	mTcpServer = new Stateplex::TcpServer(this, reinterpret_cast<sockaddr *>(&address), sizeof(address));
	mTcpServer->setTcpConnectionFactoryMethod(this, &EchoTcpServerActor::instantiateTcpConnection);
}

Stateplex::TcpConnection *EchoTcpServerActor::instantiateTcpConnection(const Stateplex::TcpConnection::Embryo *embryo)
{
	return new EchoTcpConnection(this, embryo);
}

EchoTcpConnection::EchoTcpConnection(Stateplex::Actor* actor, const Stateplex::TcpConnection::Embryo* embryo)
	: TcpConnection(actor, embryo)
{ }

void EchoTcpConnection::handleReady(bool readyRead, bool readyWrite)
{
	char buffer[1024];
	Stateplex::Size length;
	length = read(buffer, 1024);
	write(buffer, length);
}
