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
 * If enabled, adds this source to dispatcher, otherwise removes
 * this source from dispatcher.
 *
 * @param enabled	value that determines if source is to be enabled.
 */

void Source::setEnabled(bool enabled)
{
	if (enabled == mEnabled)
		return;

	mEnabled = enabled;
	if (mEnabled)
		mActor->dispatcher()->addSource(this);
	else
		mActor->dispatcher()->removeSource(this);
}

}
