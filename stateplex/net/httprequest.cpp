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
void HttpRequest::sendStatus(const String *status)
{
	if (mStatusSent)
		return;

	mHttpConnection->receiver()->receive("HTTP/1.0 ", 9);
	mHttpConnection->receiver()->receive(status);
	mHttpConnection->receiver()->receive("\r\n", 2);
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

	mHttpConnection->receiver()->receive(status);
	mHttpConnection->receiver()->receive("\r\n", 2);
	mStatusSent = true;
}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(const String *name, const String *value)
{
	mHttpConnection->receiver()->receive(name);
	mHttpConnection->receiver()->receive(":", 1);
	mHttpConnection->receiver()->receive(value);
	mHttpConnection->receiver()->receive("\r\n", 2);
}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(Buffer<> *name, Buffer<> *value)
{
	mHttpConnection->receiver()->receive(name);
	mHttpConnection->receiver()->receive(":", 1);
	mHttpConnection->receiver()->receive(value);
	mHttpConnection->receiver()->receive("\r\n", 2);
}

/**
 * Sends part of the HTTP message body.
 */
void HttpRequest::sendData(const String *data)
{
	mData.append(data);
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

	if (!mStatusSent) {
		String status = String::reference("200 OK", 6);
		sendStatus(&status);
	}

	Size length = snprintf(buffer, sizeof(buffer), "%lu", (long unsigned)mData.length());
	String name = String::reference("Content-Length", 14);
	String value = String::reference(buffer, length);
	sendHeader(&name, &value);
	mHttpConnection->receiver()->receive("\r\n", 2);
	mHttpConnection->receiver()->receive(&mData);
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
	sendStatus(mStatus);
	if (mBody) {
		sendData(mBody);
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

