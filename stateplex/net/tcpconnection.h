/*
 * Stateplex - A server-side actor model library.
 *
 * net/tcpconnection.h
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

#ifndef INCLUDED_STATEPLEX_TCP_CONNECTION_H
#define INCLUDED_STATEPLEX_TCP_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>

#include "../core/iosource.h"

namespace Stateplex {

class TcpServer;
class TcpConnectionEmbryo;

/**
 * @brief Class TcpConnection.
 *
 * TcpConnection class is used to 
 * handle the tcp connections. One server can have
 * multiple tcp connections and one tcp connection has
 * sender and receiver.
 */


class TcpConnection : public IoSource {
	/* TODO: Save the peer address */

	int connect(const struct sockaddr *address, socklen_t length);

public:
	template<typename T> TcpConnection(Actor *actor, const struct sockaddr *address, socklen_t length, T *handlerObject, void (T::*handlerFunction)(IoSource *source));
	template<typename T> TcpConnection(Actor *actor, const TcpConnectionEmbryo *embryo, T *handlerObject, void (T::*handlerFunction)(IoSource *source));
};

/**
 * @brief Class TcpConnectionEmbryo
 *
 * TcpConnectionEmbryo class is used to store data so that the creation of the
 * tcp connection is possible.
 */

class TcpConnectionEmbryo {
	friend class TcpConnection;

	int mFd;

public:
	TcpServer *mTcpServer;
	const struct sockaddr *mAddress;
	socklen_t mAddressLength;

	TcpConnectionEmbryo(TcpServer *server, int fd, const struct sockaddr *mAddress, socklen_t mAddressLength);
};

}

/*** Inline implementations ***/

namespace Stateplex {

/**
 * Constructor for class TcpConnection.
 * Initializes a new instance of TcpConnection and
 * also connects to a specified address and socket.
 * 
 * @param *actor		actor that is part of the connection.
 * @param *address		the address struct that contains socket, ip etc.
 * @param length		the length of the address.
 * @param *handlerObject	handler object that is using this constructor.
 * @param *handlerFunction	given function from handler object.
 */

template<typename T> TcpConnection::TcpConnection(Actor *actor, const struct sockaddr *address, socklen_t length, T *handlerObject, void (T::*handlerFunction)(IoSource *source))
	: IoSource(actor, handlerObject, handlerFunction)
{
	int fd;

	fd = connect(address, length);
	setFd(fd);
}

/**
 * Constructor for class TcpConnection.
 * Initializes a new instance of TcpConnection 
 *
 * @param *actor		actor that is part of the connection.
 * @param *embryo		data storage for tcp connection.
 * @param *handlerObject	handler object that is using this constructor.
 * @param *handlerFunction	given function from handler object.
 */

template<typename T> TcpConnection::TcpConnection(Actor *actor, const TcpConnectionEmbryo *embryo, T *handlerObject, void (T::*handlerFunction)(IoSource *source))
		: IoSource(actor, embryo->mFd, handlerObject, handlerFunction)
{ }

/**
 * Constructor for class TcpConnectionEmbryo.
 * Initializes a new instance of TcpConnectionEmbryo.
 *
 * @param *server		server that is part of the connection.
 * @param fd			file descriptor.
 * @param *address		the address struct that contains socket, ip etc.
 * @param addressLength		the length of the address.
 */

inline TcpConnectionEmbryo::TcpConnectionEmbryo(TcpServer *server, int fd, const struct sockaddr *address, socklen_t addressLength)
	: mTcpServer(server), mFd(fd), mAddress(address), mAddressLength(addressLength)
{ }

}

#endif
