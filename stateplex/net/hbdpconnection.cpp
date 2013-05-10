/*
 * Stateplex - A server-side actor model library.
 *
 * net/hbdpconnection.cpp
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

#include "hbdpconnection.h"

namespace Stateplex {

bool HbdpConnection::HbdpRequest::receiveHeader(Buffer *name, Buffer *value)
{
	return true;
}

bool HbdpConnection::HbdpRequest::receiveData(Buffer *data)
{
	mHbdpConnection->mReceiver->receive(data);
	return true;
}

void HbdpConnection::HbdpRequest::receiveEnd()
{
	mHbdpConnection->handleEnd();
}

void HbdpConnection::HbdpRequest::receiveAbort()
{
	// TODO: Handle failure.
}

HbdpConnection::HbdpRequest::HbdpRequest(const HttpRequest::Embryo *embryo, HbdpConnection *hbdpConnection)
	: HttpRequest(embryo), mHbdpConnection(hbdpConnection)
{ }

HttpRequest *HbdpConnection::instantiateHttpRequest(const HttpRequest::Embryo *embryo, Size serialNumber, bool close)
{
	if (mSerialNumber != serialNumber) {
		return new SimpleHttpRequest(embryo, "410 Gone");
	}

	mSerialNumber++;

	if (mHbdpRequest)
		endRequest();

	if (close)
		return new SimpleHttpRequest(embryo, "200 OK");

	mHbdpRequest = new HbdpRequest(embryo, this);
	mEndReceived = false;

	return mHbdpRequest;
}

void HbdpConnection::handleEnd()
{
	mEndReceived = true;
	if (mOut.length() > 0) {
		mHbdpRequest->sendData(&mOut);
		mOut.poppedAll();
		endRequest();
	}
}

void HbdpConnection::endRequest() {
	mHbdpRequest->sendEnd();
	//delete mHbdpRequest;
	mHbdpRequest = 0;
}

HbdpServer *HbdpConnection::hbdpServer() const
{
	return mHbdpServer;
}

void HbdpConnection::receiveEnd()
{
	// TODO
}

bool HbdpConnection::receive(const String *string)
{
	if (!mHbdpRequest || !mEndReceived) {
		mOut.append(string);
		return true;
	}

	mHbdpRequest->sendData(string);
	endRequest();

	return true;
}

bool HbdpConnection::receive(Buffer *buffer)
{
	if (!mHbdpRequest || !mEndReceived) {
		mOut.append(buffer);
		return true;
	}

	mHbdpRequest->sendData(buffer);
	endRequest();

	return true;
}

}
