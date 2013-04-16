/*
 * Stateplex - A server-side actor model library.
 *
 * net/hbdpserver.cpp
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

#include "hbdpserver.h"
#include "hbdpconnection.h"

namespace Stateplex {

HttpRequest *HbdpServer::instantiateHttpRequest(const HttpRequest::Embryo *embryo)
{
	WriteBuffer<> *uri = embryo->uri->region(mPath->length(), embryo->uri->length() - mPath->length());
	Array<WriteBuffer<> *> *elements = uri->split('/', 3);
	delete uri;

	if (elements->length() == 0) {
		String *id = String::copy(allocator(), "abcdef");
		HbdpConnection::Embryo connectionEmbryo(this, id);
		HbdpConnection *connection = mConnectionFactoryMethod.invoke(&connectionEmbryo);
		mConnections.addTail(connection);
		return new SimpleHttpRequest(embryo->httpConnection, "200 OK"); // TODO: plus id
	} else if (elements->length() == 2) {
		for (HbdpConnection *connection = mConnections.first(); connection; connection = mConnections.next(connection)) {
			if (elements->element(0)->equals(connection->id())) {
				elements->destroy(allocator());
				return connection->instantiateHttpRequest(embryo, 0);
			}
		}
	}

	elements->destroy(allocator());
	return new SimpleHttpRequest(embryo->httpConnection, "404 Not Found");
}

}
