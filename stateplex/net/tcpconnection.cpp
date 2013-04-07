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

#include "tcpconnection.h"
#include "tcpserver.h"

namespace Stateplex {

/**
 * Function that connects to a specified address and socket.
 * The function takes needed parameters for making a connection.
 * Function not ready yet.
 *
 * @param *address	struct that contains connection information such as ip, socket.
 * @param length	length of the address.
 * @return		-1.
 */

int TcpConnection::connect(const struct sockaddr *address, socklen_t length)
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

	return -1;
}

}

