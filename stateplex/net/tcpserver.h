/*
 * Stateplex - A server-side actor model library.
 *
 * net/tcpserver.h
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

#ifndef INCLUDED_STATEPLEX_TCP_SERVER_H
#define INCLUDED_STATEPLEX_TCP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>

#include "../core/source.h"
#include "../core/factorymethod.h"

namespace Stateplex {

class TcpConnection;
class TcpConnectionEmbryo;

/** 
 * @brief Class TcpServer.
 *
 * TcpServer is inherited from class Source. Handles incoming connections and
 * creates tcp connections.
 */

class TcpServer : public Source {
	FactoryMethod<TcpConnection> mConnectionFactoryMethod;

	int createServerSocket(const struct sockaddr *address, socklen_t length);

protected:
	void handleReady(bool readyToRead, bool readyToWrite);

public:
	TcpServer(Actor *actor, const struct sockaddr *address, socklen_t length);
	virtual ~TcpServer();

	template<typename T> void setTcpConnectionFactoryMethod(T *object, TcpConnection* (T::*function)(const TcpConnectionEmbryo *embryo) = 0);
};

}

/*** Inline implementations ***/

#include "../core/source.h"

namespace Stateplex {
	
/** 
 * Constructor for TcpServer class.
 *
 * @param *actor	actor that is part of this server.
 * @param *address	struct that contains connection information.
 * @param length	length of the address.
 */

inline TcpServer::TcpServer(Actor *actor, const struct sockaddr *address, socklen_t length)
	: Source(actor, -1, true, false)
{
	setFd(createServerSocket(address, length));
}

/** 
 * Destructor for TcpServer class.
 */
 
inline TcpServer::~TcpServer()
{ }

/** 
 * Sets the factory method to be used for tcp connection creation.
 *
 * @param *object	handler object.
 * @param *function	points to function that is used in tcp connection creation.
 */
 
template<typename T>
void TcpServer::setTcpConnectionFactoryMethod(T *object, TcpConnection* (T::*function)(const TcpConnectionEmbryo *embryo))
{
	mConnectionFactoryMethod.set(object, function);
	setHandled(true);
}

}
#endif
