/** @file
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.15 2001/03/19 15:29:37 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include <stdarg.h>
#include "pa_pool.h"

class String;
class Value;

#ifdef WIN32

#define open _open
#define close _close
#define read _read
#define stat _stat

#define vsnprintf __vsnprintf 
#define snprintf __snprintf

int __vsnprintf(char *, size_t, const char *, va_list);
int __snprintf(char *, size_t, const char *, ...);

#endif

/**
	read specified file using pool, 
	if fail_on_read_problem is true[default] throws an exception
*/
char *file_read(Pool& pool, const char *fname, bool fail_on_read_problem=true);

/**
	scans for \a delim[default \n] in \a *row_ref, 
	\return piece of line before it or end of string, if no \a delim found
	assigns \a *row_ref to point right after delimiter if there were one
	or to zero if no \a delim were found.
*/
char *getrow(char **row_ref,char delim='\n');
//char *lsplit(char *,char);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);
char *format(Pool& pool, double value, char *fmt);

#ifndef max
inline int max(int a,int b) { return a>b?a:b; }
inline int min(int a,int b){ return a<b?a:b; }
#endif

size_t stdout_write(const char *buf, size_t size);

const char *unescape_chars(Pool& pool, const char *cp, int len);

const String& attributed_meaning_string(Value *meaning);

#endif
