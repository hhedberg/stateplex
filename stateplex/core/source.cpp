/*
 * Stateplex - A server-side actor model library.
 *
 * core/source.h
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

#include "source.h"

namespace Stateplex {
	
/** 
 * Function that checks if a dispatcher is using source, if not set source to a dispatcher.
 * If the source has not been dispatched then it will be removed.
 */

void Source::manageDispatching()
{
	bool shouldDispatch = mEnabled && mFd != -1;
	if (shouldDispatch == mDispatched)
		return;

	mDispatched = shouldDispatch;
	if (mDispatched) {
		mActor->dispatcher()->addSource(this);
	} else {
		mActor->dispatcher()->removeSource(this);
	}
}

void Source::setMode(bool readable, bool writable)
{
	if (mReadable == readable && mWritable == writable)
		return;

	mReadable = readable;
	mWritable = writable;
	if (mDispatched)
		actor()->dispatcher()->updateSource(this);
}

}
