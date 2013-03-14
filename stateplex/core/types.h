/*
 * Stateplex - A server-side actor model library.
 *
 * core/types.h
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

#ifndef INCLUDED_STATEPLEX_TYPES_H
#define INCLUDED_STATEPLEX_TYPES_H

#include <stdint.h>
#include <cstddef>

namespace Stateplex {

typedef uint16_t Size16;
typedef uint32_t Word;
typedef std::size_t Size;
typedef ::ssize_t SignedSize;

const Size SIZE_ERROR = static_cast<Size>(-1);

}

#endif
