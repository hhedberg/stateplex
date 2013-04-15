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

bool HbdpConnection::HbdpRequest::receiveHeader(Buffer<> *name, Buffer<> *value)
{
	return true;
}

Size HbdpConnection::HbdpRequest::receiveData(Buffer<> *data)
{
	mHbdpConnection->mIn.append(data);
	mHbdpConnection->receiveData();
	return data->length();
}

void HbdpConnection::HbdpRequest::receiveEnd()
{
	if (mHbdpConnection->mOut.length() > 0) {
		sendData(&mHbdpConnection->mOut);
		mHbdpConnection->mOut.poppedAll();
		sendEnd();
	}
}

void HbdpConnection::HbdpRequest::receiveAbort()
{
	// TODO: Handle failure.
}

HbdpConnection::HbdpRequest::HbdpRequest(HttpConnection *httpConnection, HbdpConnection *hbdpConnection)
	: HttpRequest(httpConnection), mHbdpConnection(hbdpConnection)
{ }

void HbdpConnection::handleRequest(HbdpRequest *hbdpRequest, Size32 serialNumber)
{
	if (mSerialNumber != serialNumber) {
		const char status[] = "410 Gone";
		hbdpRequest->sendStatus(status, sizeof(status));
		hbdpRequest->sendEnd();
	}
	mSerialNumber++;

	if (mHbdpRequest)
		endRequest();
	mHbdpRequest = hbdpRequest;
}

void HbdpConnection::endRequest() {
	mHbdpRequest->sendEnd();
	delete mHbdpRequest;
	mHbdpRequest = 0;
}

void HbdpConnection::close()
{
	// TODO
}

HbdpServer *HbdpConnection::hbdpServer() const
{
	return mHbdpServer;
}

Size HbdpConnection::read(WriteBuffer<> *buffer)
{
	Size length = mIn.length();
	buffer->append(&mIn);
	mIn.poppedAll();
	return length;
}

void HbdpConnection::write(Buffer<> *data)
{
	if (!mHbdpRequest) {
		mOut.append(data);
		return;
	}

	mHbdpRequest->sendData(data);
	endRequest();
}

void HbdpConnection::write(const String *data)
{
	if (!mHbdpRequest) {
		mOut.append(data);
		return;
	}

	mHbdpRequest->sendData(data->chars(), data->length());
	endRequest();
}

void HbdpConnection::write(const char *data, Size dataLength)
{
	if (!mHbdpRequest) {
		mOut.append(data);
		return;
	}

	mHbdpRequest->sendData(data, dataLength);
	endRequest();
}

}
