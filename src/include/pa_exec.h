/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000,2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exec.h,v 1.7 2002/02/08 08:30:13 paf Exp $
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

/// @return exit code
int pa_exec(const String& file_spec, 
			const Hash *env, ///< 0 | Hash of String
			const Array *argv, ///< 0 | Array of command line arguments
			const String& in, String& out, String& err);

#endif
