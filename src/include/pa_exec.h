/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000,2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

static const char * const IDENT_EXEC_H="$Date: 2004/02/11 15:33:14 $";

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"
#include "pa_value.h"
#include "pa_charset.h"

struct PA_exec_result {
	int status; ///< exit code
	String out;
	String err;

	PA_exec_result(): status(0) {}
};

PA_exec_result pa_exec(
			bool forced_allow, ///< allow execution regardles of --disable-execs
			const String& file_spec, 
			const HashStringString* env, ///< 0 | Hash of String
			const ArrayString& argv, ///< 0 | Array of command line arguments
			String& in);

#endif
