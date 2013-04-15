/*
 * Stateplex - A server-side actor model library.
 *
 * net/httpconnection.cpp
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

#include "httpconnection.h"
#include "httpserver.h"
#include "httprequest.h"

namespace Stateplex {

void HttpConnection::close()
{
	bool keepAlive = mKeepAlive;

	if (mHttpRequest)
		delete mHttpRequest;
}

void HttpConnection::handleReady(bool readyToRead, bool readyToWrite)
{
	Size length;
	if (readyToRead) {
		length = read(&mInputBuffer);
		if (hasReachedEof()) {
			if (mHttpRequest) {
				mHttpRequest->receiveAbort();
				mHttpRequest = 0;
			}
			mKeepAlive = 0;
			close();
		}
		if (length == 0)
			return;

		int found;
		do {
			found = process();
		} while (found == 1);

		if (found == -1) {
			mKeepAlive = 0;
			close();
			return;
		}

		/* TODO: update inactivity information */
	}
}

int HttpConnection::process()
{
	int found;
	Size processed;

	switch (mState) {
	case STATE_METHOD: {
		found = locateRegion(' ', '\r', &mMethod);
		if (found == 1)
			mState = STATE_URI;
	} break;
	case STATE_URI: {
		found = locateRegion(' ', '\r', &mUri);
		if (found == 1)
			mState = STATE_VERSION;
	} break;
	case STATE_VERSION: {
		WriteBuffer<> *version;
		found = locateRegion('\r', ' ', &version);
		if (found == 1) {
			HttpRequest::Embryo embryo(this, mMethod, mUri);
			mHttpRequest = mHttpServer->instantiateHttpRequest(&embryo);
			mState = STATE_REQUEST_LINE_EOL;
		}
	} break;
	case STATE_REQUEST_LINE_EOL:
		/* TODO: from this on... */
	case STATE_KEY:
	case STATE_VALUE:
	case STATE_HEADER_EOL:
	case STATE_REQUEST_EOL:
	case STATE_DATA: {
		if (mInputBuffer.length() > mDataLeft) {
			Buffer<> *buffer = mInputBuffer.region(0, mDataLeft);
			processed = mHttpRequest->receiveData(buffer);
			delete buffer;
		} else
			processed = mHttpRequest->receiveData(&mInputBuffer);
		mInputBuffer.popped(processed);
		mDataLeft -= processed;
		if (mDataLeft == 0) {
			mHttpRequest->receiveEnd();
			mHttpRequest = 0;
			mState = STATE_METHOD;
		}
	} break;
	}

	return found;
}

int HttpConnection::locateRegion(const char success, const char fail, WriteBuffer<> **regionReturn)
{
	int found = locateChar(success, fail);
	if (found == 1) {
		Size length = mInputBufferIterator.offset();
		*regionReturn = mInputBuffer.region(0, length);
		mInputBuffer.popped(length + 1);
	}
	return found;
}

int HttpConnection::locateChar(const char success, const char fail)
{
	while (mInputBufferIterator.left() > 0) {
		const char c = mInputBufferIterator.current();
		if (c == success)
			return 1;
		if (c == fail)
			return -1;
		mInputBufferIterator.advance();
	}

	return 0;
}

bool HttpConnection::eatChars(const char eaten)
{
	for (bool found = false; mInputBufferIterator.left() > 0 && mInputBufferIterator.current() == eaten; mInputBufferIterator.advance())
		found = true;
}

}
