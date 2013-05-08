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
#include "../core/writebuffer.h"

namespace Stateplex {

class HttpRequest;
class HttpRequestEmbryo;
class TcpServer;
class HttpServer;

/**
 * HTTP connection between the HttpServer and a client.
 */
class HttpConnection : public Object, public Receiver {
	HttpServer *mHttpServer;
	Receiver *mReceiver;
	WriteBuffer mInputBuffer;
	Buffer::Iterator mInputBufferIterator;
	WriteBuffer *mMethod;
	WriteBuffer *mUri;
	HttpRequest *mHttpRequest;

	enum State {
		STATE_PRE_METHOD = 0,
		STATE_METHOD,
		STATE_PRE_URI,
		STATE_URI,
		STATE_PRE_VERSION,
		STATE_VERSION,
		STATE_REQUEST_LINE_EOL,
		STATE_PRE_KEY,
		STATE_KEY,
		STATE_PRE_VALUE,
		STATE_VALUE,
		STATE_HEADER_EOL,
		STATE_HEADERS_END_EOL,
		STATE_DATA
	} mState;

	enum ProcessResult {
		PROCESS_RESULT_NOT_FOUND,
		PROCESS_RESULT_FOUND,
		PROCESS_RESULT_ERROR,
		PROCESS_RESULT_REQUEST_END
	};

	unsigned int mKeepAlive : 1;

	void close();
	void receive();
	ProcessResult process();
	bool eatSpaces();
	bool handleVersion(Buffer *version);
	void handleHeader(Buffer *name, Buffer *value);
	ProcessResult locateChar(const char success, const char fail);
	ProcessResult locateRegion(const char success, const char fail, WriteBuffer **regionReturn);

protected:
	virtual void receiveEnd();
	virtual void receive(const String *string);
	virtual void receive(Buffer *buffer);

public:
	HttpConnection(Actor *actor, HttpServer *server, Receiver *receiver);

	HttpServer *httpServer() const;
	Receiver *receiver() const;
};

}

/*** Inline implementations ***/

namespace Stateplex {

inline HttpConnection::HttpConnection(Actor *actor, HttpServer *server, Receiver *receiver)
	: Object(actor), mHttpServer(server), mReceiver(receiver), mInputBuffer(actor), mInputBufferIterator(&mInputBuffer), mMethod(0), mUri(0), mHttpRequest(0), mState(STATE_PRE_METHOD), mKeepAlive(0)
{ }

inline HttpServer *HttpConnection::httpServer() const
{
	return mHttpServer;
}

inline Receiver *HttpConnection::receiver() const
{
	return mReceiver;
}

}

#endif
