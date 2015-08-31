/*
 *
 *  Embedded Linux library
 *
 *  Copyright (C) 2011-2015  Intel Corporation. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __ELL_UUID_H
#define __ELL_UUID_H

#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t L_UUID_NAMESPACE_DNS[];
extern const uint8_t L_UUID_NAMESPACE_URL[];
extern const uint8_t L_UUID_NAMESPACE_OID[];
extern const uint8_t L_UUID_NAMESPACE_X500[];

bool l_uuid_v3(const uint8_t nsid[16], const void *name, size_t name_size,
			uint8_t out_uuid[16]);
bool l_uuid_v4(uint8_t out_uuid[16]);
bool l_uuid_v5(const uint8_t nsid[16], const void *name, size_t name_size,
			uint8_t out_uuid[16]);

#ifdef __cplusplus
}
#endif

#endif /* __ELL_UTIL_H */
