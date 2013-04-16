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
#include "../core/writebuffer.h"
#include "httprequest.h"

namespace Stateplex {

class HbdpServer;

/**
 * A connection through HTTP Bidirectional Protocol.
 */
class HbdpConnection : public Object, public ListItem {
	friend class HbdpServer;

	class HbdpRequest : public HttpRequest {
		HbdpConnection *mHbdpConnection;

	protected:
		virtual bool receiveHeader(Buffer<> *name, Buffer<> *value);
		virtual bool receiveData(Buffer<> *data);
		virtual void receiveEnd();
		virtual void receiveAbort();

	public:
		HbdpRequest(HttpConnection *httpConnection, HbdpConnection *hbdpConnection);
	};

	HbdpServer *mHbdpServer;
	HbdpRequest *mHbdpRequest;
	String *mId;
	Size32 mSerialNumber;
	WriteBuffer<> mOut;

	HttpRequest *instantiateHttpRequest(const HttpRequest::Embryo *embryo, Size serialNumber);
	void handleEnd();
	void endRequest();

protected:
	virtual void receiveData(Buffer<> *data) = 0;
	virtual void receiveClose() = 0;

public:
	class Embryo {
		friend class HbdpConnection;
		friend class HbdpServer;

		HbdpServer *mHbdpServer;
		const String *mId;

		Embryo(HbdpServer *hbdpServer, const String *id);
	};

	HbdpConnection(Actor *actor, const Embryo *embryo);
	virtual ~HbdpConnection();

	void close();
	HbdpServer *hbdpServer() const;
	const String *id() const;
	void write(Buffer<> *data);
	void write(const String *data);
	void write(const char *data, Size dataLength);
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline HbdpConnection::HbdpConnection(Actor *actor, const Embryo *embryo)
	: Object(actor), mHbdpServer(embryo->mHbdpServer), mHbdpRequest(0), mSerialNumber(0), mOut(actor)
{ }

inline HbdpConnection::~HbdpConnection()
{ }

inline const String *HbdpConnection::id() const
{
	return mId;
}

inline HbdpConnection::Embryo::Embryo(HbdpServer *hbdpServer, const String *id)
	: mHbdpServer(hbdpServer), mId(id)
{ }

}

#endif
