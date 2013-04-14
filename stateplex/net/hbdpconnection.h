/*
 * Stateplex - A server-side actor model library.
 *
 * net/hbdpconnection.h
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

#ifndef INCLUDED_STATEPLEX_HBDP_CONNECTION_H
#define INCLUDED_STATEPLEX_HBDP_CONNECTION_H

#include "../core/list.h"
#include "../core/buffer.h"

namespace Stateplex {

class HbdpServer;

/**
 * A connection through HTTP Bidirectional Protocol.
 */
class HbdpConnection : public Object, public ListItem {
	HbdpServer *mHbdpServer;

protected:
	virtual void receive() = 0;

public:
	class Embryo {
		friend class HbdpConnection;

		HbdpServer *mHbdpServer;
	};

	HbdpConnection(Actor *actor, const Embryo *embryo);
	virtual ~HbdpConnection();

	void close();
	HbdpServer *hbdpServer() const;
	Size read(Buffer<> *buffer);
	void write(Buffer<> data);
	void write(String data);
	void write(const char *data, Size dataLength);
};

}

/*** Inline implementations ***/

namespace Stateplex {

HbdpConnection::HbdpConnection(Actor *actor, const Embryo *embryo)
	: Object(actor), mHbdpServer(embryo->mHbdpServer)
{ }

HbdpConnection::~HbdpConnection()
{ }

}

#endif
