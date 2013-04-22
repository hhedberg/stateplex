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
		} while (found == 1 && mInputBuffer.length() > 0);

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
	int found = 0;

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
			delete mMethod;
			delete mUri;
			mState = STATE_REQUEST_LINE_EOL;
		}
	} break;
	case STATE_REQUEST_LINE_EOL: {
		if (mInputBuffer.pop() != '\n')
			return -1;
		found = 1;
		mState = STATE_KEY_FIRST;
	} break;
	case STATE_KEY_FIRST:
		if (mInputBuffer.charAt(0) == '\r') {
			mInputBuffer.pop();
			found = 1;
			mState = STATE_REQUEST_EOL;
			break;
		}
		mState = STATE_KEY;
		// No break.
	case STATE_KEY: {
		found = locateRegion(':', '\r', &mMethod);
		if (found == 1)
			mState = STATE_VALUE;
	} break;
	case STATE_VALUE: {
		found = locateRegion('\r', '\n', &mUri);
		if (found == 1) {
			if (mMethod->equals("Content-Length")) {
				String *s = mUri->asString();
				mDataLeft = atoi(s->chars());
				s->destroy(allocator());
			}
			mHttpRequest->receiveHeader(mMethod, mUri);
			delete mMethod;
			delete mUri;
			mState = STATE_HEADER_EOL;
		}
	} break;
	case STATE_HEADER_EOL: {
		if (mInputBuffer.pop() != '\n')
			return -1;
		found = 1;
		mState = STATE_KEY_FIRST;
	} break;
	case STATE_REQUEST_EOL: {
		if (mInputBuffer.pop() != '\n')
			return -1;
		found = 1;
		if (mDataLeft == 0) {
			mHttpRequest->receiveEnd();
			mHttpRequest = 0;
			mState = STATE_METHOD;
		} else {
			mState = STATE_DATA;
		}
	} break;
	case STATE_DATA: {
		bool ok;
		if (mInputBuffer.length() > mDataLeft) {
			WriteBuffer<> *buffer = mInputBuffer.region(0, mDataLeft);
			mDataLeft = 0;
			mInputBuffer.popped(mDataLeft);
			ok = mHttpRequest->receiveData(buffer);
			delete buffer;
		} else {
			ok = mHttpRequest->receiveData(&mInputBuffer);
			mDataLeft -= mInputBuffer.length();
			mInputBuffer.poppedAll();
		}
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
	while (mInputBufferIterator.hasCurrent()) {
		const char c = mInputBufferIterator.current();
		if (c == success)
			return 1;
		if (c == fail)
			return -1;
		mInputBufferIterator.advance();
	}

	return 0;
}

}
