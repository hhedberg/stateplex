/*
 * Stateplex - A server-side actor model library.
 *
 * examples/echohbdpserver/echohbdpserver.h
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

#ifndef INCLUDED_ECHO_HBDP_SERVER_H
#define INCLUDED_ECHO_HBDP_SERVER_H

#include <stateplex/core/actor.h>
#include <stateplex/net/hbdpconnection.h>

class EchoActor : public Stateplex::Actor {
	Stateplex::HbdpServer *mHbdpServer;

public:
	EchoActor(Stateplex::Dispatcher *dispatcher);

	Stateplex::HbdpConnection *instantiateHbdpConnection(const Stateplex::HbdpConnection::Embryo *embryo);
};

class EchoHbdpConnection : public Stateplex::HbdpConnection {

protected:
	virtual void receiveData(Stateplex::Buffer<> *data);
	virtual void receiveClose();

public:
	EchoHbdpConnection(Stateplex::Actor *actor, const Stateplex::HbdpConnection::Embryo *embryo);
};

#endif
