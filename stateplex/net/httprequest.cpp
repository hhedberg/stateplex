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

#include "httprequest.h"

namespace Stateplex {

/**
 * Sends the HTTP status line.
 * The status line can be sent only once. Any subsequent calls of sendStatus() functions will be ignored.
 */
void HttpRequest::sendStatus(const char *status, size_t statusLength)
{

}

/**
 * Sends the HTTP status line.
 * The status line can be sent only once. Any subsequent calls of sendStatus() functions will be ignored.
 */
void HttpRequest::sendStatus(const Buffer<> *status)
{

}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(const char *name, Size nameLength, const char *value, Size valueLength)
{

}

/**
 * Sends the HTTP header line.
 */
void HttpRequest::sendHeader(const Buffer<> *name, const Buffer<> *value)
{

}

/**
 * Sends part of the HTTP message body.
 */
void HttpRequest::sendData(const char *data, Size dataLength)
{

}

/**
 * Sends part of the HTTP message body.
 */
void HttpRequest::sendData(const Buffer<> *data)
{

}

/**
 * Indicates that the HTTP response is now sent.
 */
void HttpRequest::sendEnd()
{

}

}

