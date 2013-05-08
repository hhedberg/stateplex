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
	if (mHttpRequest)
		delete mHttpRequest;
}

void HttpConnection::receiveEnd()
{
	if (mHttpRequest) {
		mHttpRequest->receiveAbort();
		mHttpRequest = 0;
	}
	mKeepAlive = 0;
	close();
}

void HttpConnection::receive(const String *string)
{
	mInputBuffer.append(string);
	receive();
}

void HttpConnection::receive(Buffer *buffer)
{
	mInputBuffer.append(buffer);
	receive();
}

void HttpConnection::receive()
{
	ProcessResult result;
	do {
		result = process();
	} while (result == PROCESS_RESULT_FOUND && mInputBuffer.length() > 0);

	if (result == PROCESS_RESULT_ERROR) {
		mKeepAlive = 0;
		close();
		return;
	} else if (result == PROCESS_RESULT_REQUEST_END) {
		if (mKeepAlive)
			mState = STATE_PRE_METHOD;
		else
			close();
	}

	/* TODO: update inactivity information */
}

HttpConnection::ProcessResult HttpConnection::process()
{
	ProcessResult result = PROCESS_RESULT_NOT_FOUND;

	switch (mState) {
	case STATE_PRE_METHOD:
		if (eatSpaces())
			return PROCESS_RESULT_NOT_FOUND;
		mState = STATE_METHOD;
		// No break.
	case STATE_METHOD: {
		result = locateRegion(' ', '\r', &mMethod);
		if (result == PROCESS_RESULT_FOUND)
			mState = STATE_PRE_URI;
	} break;
	case STATE_PRE_URI:
		if (eatSpaces())
			return PROCESS_RESULT_NOT_FOUND;
		mState = STATE_URI;
		// No break;
	case STATE_URI: {
		result = locateRegion(' ', '\r', &mUri);
		if (result == PROCESS_RESULT_FOUND)
			mState = STATE_PRE_VERSION;
	} break;
	case STATE_PRE_VERSION:
		if (eatSpaces())
			return PROCESS_RESULT_NOT_FOUND;
		mState = STATE_VERSION;
		// No break;
	case STATE_VERSION: {
		WriteBuffer *version;
		result = locateRegion('\r', ' ', &version);
		if (result == PROCESS_RESULT_FOUND) {
			if (!handleVersion(version))
				return PROCESS_RESULT_ERROR;
			HttpRequest::Embryo embryo(this, mMethod, mUri, version);
			mHttpRequest = mHttpServer->instantiateHttpRequest(&embryo);
			if (!mHttpRequest)
				mHttpRequest = new SimpleHttpRequest(&embryo, "501 Not Implemented");
			delete mMethod;
			delete mUri;
			delete version;
			mState = STATE_REQUEST_LINE_EOL;
		}
	} break;
	case STATE_REQUEST_LINE_EOL: {
		if (mInputBuffer.pop() != '\n')
			return PROCESS_RESULT_ERROR;
		result = PROCESS_RESULT_FOUND;
		mState = STATE_PRE_KEY;
	} break;
	case STATE_PRE_KEY:
		if (eatSpaces())
			return PROCESS_RESULT_NOT_FOUND;
		if (mInputBuffer.charAt(0) == '\r') {
			mInputBuffer.pop();
			result = PROCESS_RESULT_FOUND;
			mState = STATE_HEADERS_END_EOL;
			break;
		}
		mState = STATE_KEY;
		// No break.
	case STATE_KEY: {
		result = locateRegion(':', '\r', &mMethod);
		if (result == PROCESS_RESULT_FOUND)
			mState = STATE_PRE_VALUE;
	} break;
	case STATE_PRE_VALUE:
		if (eatSpaces())
			return PROCESS_RESULT_NOT_FOUND;
		mState = STATE_VALUE;
		// No break;
	case STATE_VALUE: {
		result = locateRegion('\r', '\n', &mUri);
		if (result == PROCESS_RESULT_FOUND) {
			handleHeader(mMethod, mUri);
			mHttpRequest->receiveHeader(mMethod, mUri);
			delete mMethod;
			delete mUri;
			mState = STATE_HEADER_EOL;
		}
	} break;
	case STATE_HEADER_EOL: {
		if (mInputBuffer.pop() != '\n')
			return PROCESS_RESULT_ERROR;
		result = PROCESS_RESULT_FOUND;
		mState = STATE_PRE_KEY;
	} break;
	case STATE_HEADERS_END_EOL: {
		if (mInputBuffer.pop() != '\n')
			return PROCESS_RESULT_ERROR;
		result = PROCESS_RESULT_FOUND;
		if (mHttpRequest->mDataLeft == 0) {
			mHttpRequest->receiveEnd();
			mHttpRequest = 0;
			result = PROCESS_RESULT_REQUEST_END;
		} else {
			mState = STATE_DATA;
		}
	} break;
	case STATE_DATA: {
		if (mInputBuffer.length() > mHttpRequest->mDataLeft) {
			WriteBuffer *buffer = mInputBuffer.region(0, mHttpRequest->mDataLeft);
			mInputBuffer.popped(mHttpRequest->mDataLeft);
			mHttpRequest->mDataLeft = 0;
			mHttpRequest->receiveData(buffer);
			delete buffer;
		} else {
			mHttpRequest->receiveData(&mInputBuffer);
			mHttpRequest->mDataLeft -= mInputBuffer.length();
			mInputBuffer.poppedAll();
		}
		if (mHttpRequest->mDataLeft == 0) {
			mHttpRequest->receiveEnd();
			mHttpRequest = 0;
			result = PROCESS_RESULT_REQUEST_END;
		}
	} break;
	}

	return result;
}

bool HttpConnection::eatSpaces()
{
	while (mInputBuffer.length() > 0) {
		char c = mInputBuffer.peek();
		if (c != ' ' && c != '\t')
			return false;
		mInputBuffer.pop();
	}

	return true;
}

bool HttpConnection::handleVersion(Buffer *version)
{
	if (version->equals("HTTP/1.0")) {
		mKeepAlive = 0;
	} else if (version->equals("HTTP/1.1")) {
		mKeepAlive = 1;
	} else
		return false;

	return true;
}

void HttpConnection::handleHeader(Buffer *name, Buffer *value)
{
	if (name->equals("Content-Length")) {
		String *s = value->asString();
		mHttpRequest->mDataLeft = atoi(s->chars());
		s->destroy(allocator());
	} else if (name->equals("Connection")) {
		if (value->equals("Keep-Alive"))
			mKeepAlive = 1;
	}
}

HttpConnection::ProcessResult HttpConnection::locateRegion(const char success, const char fail, WriteBuffer **regionReturn)
{
	ProcessResult result = locateChar(success, fail);
	if (result == PROCESS_RESULT_FOUND) {
		Size length = mInputBufferIterator.offset();
		*regionReturn = mInputBuffer.region(0, length);
		mInputBuffer.popped(length + 1);
	}
	return result;
}

HttpConnection::ProcessResult HttpConnection::locateChar(const char success, const char fail)
{
	while (mInputBufferIterator.hasCurrent()) {
		const char c = mInputBufferIterator.current();
		if (c == success)
			return PROCESS_RESULT_FOUND;
		if (c == fail)
			return PROCESS_RESULT_ERROR;
		mInputBufferIterator.advance();
	}

	return PROCESS_RESULT_NOT_FOUND;
}

}
