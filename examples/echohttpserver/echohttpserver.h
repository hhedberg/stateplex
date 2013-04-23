/*
 * Stateplex - A server-side actor model library.
 *
 * examples/echohttpserver/echohttpserver.h
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

#ifndef INCLUDED_ECHO_HTTP_SERVER_H
#define INCLUDED_ECHO_HTTP_SERVER_H

#include <stateplex/core/actor.h>
#include <stateplex/net/httprequest.h>

class EchoActor : public Stateplex::Actor {
	Stateplex::HttpServer *mHttpServer;

public:
	EchoActor(Stateplex::Dispatcher *dispatcher);

	Stateplex::HttpRequest *instantiateHttpRequest(const Stateplex::HttpRequest::Embryo *embryo);
};

class EchoHttpRequest : public Stateplex::HttpRequest {
	bool mDataReceived;

protected:
	virtual bool receiveHeader(Stateplex::Buffer<> *name, Stateplex::Buffer<> *value);
	virtual bool receiveData(Stateplex::Buffer<> *data);
	virtual void receiveEnd();
	virtual void receiveAbort();

public:
	EchoHttpRequest(const Stateplex::HttpRequest::Embryo *embryo);
};

#endif
