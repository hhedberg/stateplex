/*
 * Stateplex - A server-side actor model library.
 *
 * net/tcpconnection.cpp
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

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "tcpserver.h"
#include "tcpconnection.h"

namespace Stateplex {
	
/** 
 * Creates server socket.
 *
 * @param *address	struct that has information about address.
 * @param length	length of the address.
 * @return			returns -1 if fails, otherwise an int value other than -1.
 */

int TcpServer::createServerSocket(const struct sockaddr *address, socklen_t length)
{
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return -1;
	if (bind(fd, address, length) == -1 ||
	    listen(fd, 128) == -1) {
	    	::close(fd);
		return -1;
	}

	return fd;
}

/** 
 * "Creates" outgoing connection from embryo data if tcp information is correct.
 *
 * @param bool	TODO
 * @param bool	TODO
 */
 
void TcpServer::handleReady(bool /* readyToRead */, bool /* readyToWrite */)
{
	while (1) {
		sockaddr_in address;
		socklen_t length = sizeof(address);
		int socket = accept(fd(), reinterpret_cast<struct sockaddr *>(&address), &length);
		if (socket == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				perror("accept");
				abort();
			}
		}
		TcpConnectionEmbryo embryo(this, socket, reinterpret_cast<struct sockaddr *>(&address), length);
		TcpConnection *connection = mConnectionFactoryMethod.invoke(&embryo);
		if (!connection)
			::close(socket);
	}
}

}
