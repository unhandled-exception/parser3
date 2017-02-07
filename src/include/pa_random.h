/** @file
	Parser: random related functions.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_RANDOM_H
#define PA_RANDOM_H

#define IDENT_PA_RANDOM_H "$Id: pa_random.h,v 1.7 2017/02/07 22:00:35 moko Exp $"

#include "pa_types.h"

void random(void *buffer, size_t size);

static inline int _random(int top) {
	uint32_t raw;
	random(&raw, sizeof(raw));
	return int( double(raw) / ((double)UINT32_MAX+1) * top );
}

char *get_uuid_cstr();
char *get_uuid_boundary();

#endif
