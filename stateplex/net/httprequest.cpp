/*
 * Stateplex - A server-side actor model library.
 *
 * net/httprequest.cpp
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
#include <stdio.h>

#include "httprequest.h"

namespace Stateplex {

/**
 * Sends the HTTP status line.
 * The status line can be sent only once. Any subsequent calls of sendStatus() functions will be ignored.
 */
void HttpRequest::sendStatus(const char *status, size_t statusLength)
{
	if (mStatusSent)
		return;

	mHttpConnection->write("HTTP/1.0 ", 9);
	mHttpConnection->write(status, statusLength);
	mHttpConnection->write("\r\n", 2);
	mStatusSent = true;
}

/**
 * Sends the HTTP status line.
 * The status line can be sent only once. Any subsequent calls of sendStatus() functions will be ignored.
 */
void HttpRequest::sendStatus(Buffer<> *status)
{
	if (mStatusSent)
		return;

	mHttpConnection->write(status);
	mHttpConnection->write("\r\n", 2);
	mStatusSent = true;
}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(const char *name, Size nameLength, const char *value, Size valueLength)
{
	mHttpConnection->write(name, nameLength);
	mHttpConnection->write(":", 1);
	mHttpConnection->write(value, valueLength);
	mHttpConnection->write("\r\n", 2);
}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(Buffer<> *name, Buffer<> *value)
{
	mHttpConnection->write(name);
	mHttpConnection->write(":", 1);
	mHttpConnection->write(value);
	mHttpConnection->write("\r\n", 2);
}

/**
 * Sends part of the HTTP message body.
 */
void HttpRequest::sendData(const char *data, Size dataLength)
{
	mData.append(data, dataLength);
}

/**
 * Sends part of the HTTP message body.
 */
void HttpRequest::sendData(Buffer<> *data)
{
	mData.append(data);
}

/**
 * Indicates that the HTTP response is now sent.
 */
void HttpRequest::sendEnd()
{
	char buffer[128];

	if (!mStatusSent)
		sendStatus("200 OK", 6);

	Size length = snprintf(buffer, sizeof(buffer), "%lu", (long unsigned)mData.length());
	sendHeader("Content-Length", 14, buffer, length);
	mHttpConnection->write("\r\n", 2);
	mHttpConnection->write(&mData);
}

bool SimpleHttpRequest::receiveHeader(Buffer<> *name, Buffer<> *value)
{
	return true;
}

bool SimpleHttpRequest::receiveData(Buffer<> *data)
{
	return true;
}

void SimpleHttpRequest::receiveEnd()
{
	sendStatus(mStatus->chars(), mStatus->length());
	if (mBody) {
		sendData(mBody->chars(), mBody->length());
	}
	sendEnd();
	delete this;
}

void SimpleHttpRequest::receiveAbort()
{
	delete this;
}

SimpleHttpRequest::SimpleHttpRequest(const HttpRequest::Embryo *embryo, const char *status)
	: HttpRequest(embryo), mBody(0)
{
	mStatus = String::copy(allocator(), status);
}

SimpleHttpRequest::SimpleHttpRequest(const HttpRequest::Embryo *embryo, const char *status, const char *body)
	: HttpRequest(embryo)
{
	mStatus = String::copy(allocator(), status);
	mBody = String::copy(allocator(), body);
}

}

