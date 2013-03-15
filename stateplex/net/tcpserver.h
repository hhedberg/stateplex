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
 * TcpServer is inherited from class Source.
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
 * @param *actor	TODO
 * @param *address	TODO
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
 * Sets TODO.
 *
 * @param *object	TODO.
 * @param *function	function parameter.
 */
 
template<typename T>
void TcpServer::setTcpConnectionFactoryMethod(T *object, TcpConnection* (T::*function)(const TcpConnectionEmbryo *embryo))
{
	mConnectionFactoryMethod.set(object, function);
	setHandled(true);
}

}
#endif
