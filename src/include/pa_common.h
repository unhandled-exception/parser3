/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.5 2001/03/11 08:16:32 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include <stdarg.h>
#include "pa_pool.h"

#ifdef WIN32
#define vsnprintf __vsnprintf 
#define snprintf __snprintf

int __vsnprintf(char *, size_t, const char *, va_list);
int __snprintf(char *, size_t, const char *, ...);
#endif

char *file_read(Pool& pool, char *fname, bool fail_on_read_problem);

#endif
