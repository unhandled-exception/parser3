/** @file
	Parser: mutex & helpers decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_threads.h,v 1.13 2001/05/17 09:47:17 parser Exp $
*/

#ifndef PA_THREADS_H
#define PA_THREADS_H

#include "pa_config_includes.h"

#include "pa_types.h"


#ifdef MULTITHREAD

class AutoSYNCHRONIZED;

/// simple semaphore object
class Mutex {
	friend AutoSYNCHRONIZED;
private:
	uint handle;
public:
	Mutex();
	~Mutex();
private: // for AutoSYNCHRONIZED
	void acquire();
	void release();
};

extern Mutex global_mutex;

/** 
	Helper to ensure paired Mutex::acquire() and Mutex::release().

	Use it with SYNCHRONIZED macro
*/
class AutoSYNCHRONIZED {
public:
	AutoSYNCHRONIZED() { global_mutex.acquire(); }
	~AutoSYNCHRONIZED() { global_mutex.release(); }
};

/** 
	put it to first line of a function to ensure thread safety.
	@verbatim
		void someclass::somefunc(...) { SYNCHRONIZED;
			...
		}
	@endverbatim

	WARNING: don't use THROW or PTHROW with such thread safety mechanizm -
	longjump would leave global_mutex acquired, which is wrong!
*/
#	define SYNCHRONIZED AutoSYNCHRONIZED autoSYNCHRONIZED()
#else // not MULTITHREAD-ed
#	define SYNCHRONIZED /* do nothing */
#endif


#endif
