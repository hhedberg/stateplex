/*
 * Stateplex - A server-side actor model library.
 *
 * examples/echotcpserver/echotcpserveractor.h
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

#ifndef INCLUDED_ECHO_TCP_SERVER_ACTOR_H
#define INCLUDED_ECHO_TCP_SERVER_ACTOR_H

#include <stateplex/core/actor.h>
#include <stateplex/net/tcpconnection.h>

class EchoTcpServerActor : public Stateplex::Actor {
	Stateplex::TcpServer *mTcpServer;

	Stateplex::TcpConnection *instantiateTcpConnection(const Stateplex::TcpConnectionEmbryo *embryo);
	void handleTcpConnection(Stateplex::IoSource *source);

public:
	EchoTcpServerActor(Stateplex::Dispatcher *dispatcher);

};

#endif
