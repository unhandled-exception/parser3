/*
  $Id: pa_pool.h,v 1.8 2001/01/27 15:21:05 paf Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

class Pool {
public:
	Pool();
	~Pool();
    void *malloc(size_t size);
    void *calloc(size_t size);

	String *make_string() { 
		return new(this) String(this);
	}
	String *make_string(char *src) {
		return new(this) String(this, src);
	}
	Hash *make_hash() {
		return new(this) Hash(this);
	}
	Array *make_array() {
		return new(this) Array(this);
	}
	Array *make_array(int initial_rows) {
		return new(this) Array(this, initial_rows);
	}

private: //disabled

	Pool& operator = (Pool& src) { return *this; }
	Pool(Pool& src) {}
};

#endif
