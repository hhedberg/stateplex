/*
 * Stateplex - A server-side actor model library.
 *
 * net/httpconnection.h
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

#ifndef INCLUDED_STATEPLEX_HTTP_CONNECTION_H
#define INCLUDED_STATEPLEX_HTTP_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>

#include "tcpconnection.h"
#include "../core/factorymethod.h"
#include "../core/buffer.h"

namespace Stateplex {

class HttpRequest;
class HttpRequestEmbryo;
class TcpServer;
class HttpServer;

/**
 * HTTP connection between the HttpServer and a client.
 */
class HttpConnection : public TcpConnection {
	HttpServer *mHttpServer;
	Buffer<> mInputBuffer;
	Buffer<> *mMethod;
	Buffer<> *mUri;
	Size mDataLeft;
	HttpRequest *mHttpRequest;

	enum State {
		STATE_PRE_METHOD = 0,
		STATE_METHOD,
		STATE_PRE_URI,
		STATE_URI,
		STATE_PRE_VERSION,
		STATE_VERSION,
		STATE_REQUEST_LINE_EOL,
		STATE_KEY,
		STATE_PRE_VALUE,
		STATE_VALUE,
		STATE_HEADER_EOL,
		STATE_REQUEST_EOL,
		STATE_DATA
	} mState;

	unsigned int mKeepAlive : 1;

	void close();
	int process();

	bool eatChars(const char eaten);
	int locateChar(const char success, const char fail);
	int locateRegion(const char success, const char fail, Buffer<> **regionReturn);

protected:
	virtual void handleReady(bool readyToRead, bool readyToWrite);

public:
	HttpConnection(Actor *actor, HttpServer *server, const TcpConnection::Embryo *embryo);

	HttpServer *httpServer() const;
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline HttpConnection::HttpConnection(Actor *actor, HttpServer *server, const TcpConnection::Embryo *embryo)
	: TcpConnection(actor, embryo), mHttpServer(server), mInputBuffer(actor), mHttpRequest(0), mState(STATE_METHOD), mKeepAlive(0), mDataLeft(0), mMethod(0), mUri(0)
{ }

inline HttpServer *HttpConnection::httpServer() const
{
	return mHttpServer;
}


}

#endif
