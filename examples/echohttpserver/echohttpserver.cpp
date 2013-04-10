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
#include <stateplex/net/httpserver.h>
#include <stateplex/net/httprequest.h>

#include "echohttpserver.h"

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
	mHttpServer = new Stateplex::HttpServer(tcpServer);
	mHttpServer->setRequestFactoryMethod("/", this, &EchoActor::instantiateHttpRequest);
}

Stateplex::HttpRequest *EchoActor::instantiateHttpRequest(const Stateplex::HttpRequest::Embryo *embryo)
{
	Stateplex::HttpRequest *request = new EchoHttpRequest(embryo->httpConnection);

	request->sendData(embryo->method);
	request->sendData(" ", 1);
	request->sendData(embryo->uri);
	request->sendData("\n", 1);

	return request;
}

EchoHttpRequest::EchoHttpRequest(Stateplex::HttpConnection *connection)
	: HttpRequest(connection), mDataReceived(false)
{ }

bool EchoHttpRequest::receiveHeader(const Stateplex::Buffer<> *key, const Stateplex::Buffer<> *value)
{
	sendData(key);
	sendData(": ", 2);
	sendData(value);
	sendData("\n", 1);
}

Stateplex::Size EchoHttpRequest::receiveData(const Stateplex::Buffer<> *data)
{
	if (!mDataReceived) {
		sendData("\n", 1);
		mDataReceived = true;
	}
	sendData(data);
}

void EchoHttpRequest::receiveEnd()
{
	sendEnd();
	delete this;
}

void EchoHttpRequest::receiveAbort()
{
	delete this;
}
