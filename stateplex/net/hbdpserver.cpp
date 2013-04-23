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

#include <stdio.h>

#include "hbdpserver.h"
#include "hbdpconnection.h"

namespace Stateplex {

HttpRequest *HbdpServer::instantiateHttpRequest(const HttpRequest::Embryo *embryo)
{
	WriteBuffer<> *uri = embryo->uri()->region(mPath->length(), embryo->uri()->length() - mPath->length());
	Array<WriteBuffer<> *> *elements = uri->split('/', 3);
	delete uri;

	if (elements->length() == 0) {
		char buffer[128];
		Size length = snprintf(buffer, sizeof(buffer), "%lu", (long unsigned)actor()->dispatcher()->milliseconds());
		// TODO: More random id
		String *id = String::copy(allocator(), buffer, length);
		HbdpConnection::Embryo connectionEmbryo(this, id);
		HbdpConnection *connection = mConnectionFactoryMethod.invoke(&connectionEmbryo);
		mConnections.addTail(connection);
		return new SimpleHttpRequest(embryo, "200 OK", id->chars());
	} else if (elements->length() == 2) {
		for (HbdpConnection *connection = mConnections.first(); connection; connection = mConnections.next(connection)) {
			if (elements->element(0)->equals(connection->id())) {
				String *serialString = elements->element(1)->asString();
				Size serialNumber = atol(serialString->chars());
				serialString->destroy(allocator());
				elements->destroy(allocator());
				return connection->instantiateHttpRequest(embryo, serialNumber);
			}
		}
	}

	elements->destroy(allocator());
	return new SimpleHttpRequest(embryo, "404 Not Found");
}

}
