/*
  $Id: pa_pool.h,v 1.3 2001/01/26 15:33:40 paf Exp $
*/

/*

	String				Chunk0
	======				========
	head--------->[ptr, size]
	append_here-------->[ptr, size]
	link_row      ........
			.			.
			.			[ptr, size]
			...........>[link to the next chunk]

*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class Pool;

class String {
public:
	enum {
		CR_PREALLOCATED_COUNT=5,
		CR_GROW_PERCENT=60
	};

private:
	friend Pool;

	// the pool I'm allocated on
	Pool *pool;

	// last chank allocated count cache
	int curr_chunk_rows;
	struct Chunk {
		// the number of rows per chunk
		int count;
		union Row {
			// chunk item
			struct {
				char *ptr;  // pointer to the start of string fragment
				size_t size;  // length of the fragment
			} item;
			Chunk *link;  // link to the next chunk in chain
		} first[CR_PREALLOCATED_COUNT];
		// next rows are here
		Chunk *preallocated_link;
	}
		head;  // the head chunk of the chunk chain

	// next append would write to this record
	Chunk::Row *append_here;
	
	// the address of place where lies address 
	// of the link to the next chunk to allocate
	Chunk::Row *link_row;

	// new&constructors made private to enforce factory manufacturing at pool
	void *operator new(size_t size, Pool *apool);

	void construct(Pool *apool);
	String(Pool *apool) { 
		construct(apool); 
	}
	String(Pool *apool, char *src) {
		construct(apool);
		*this+=src;
	}

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand();

public:

	size_t size();
	char *c_str();
	String& operator += (char *src);
};

class Pool {
public:
	Pool();
	~Pool();
    void *alloc(size_t size);
    void *calloc(size_t size);

	String *makeString() { 
		return new(this) String(this);
	}
	String *makeString(char *src) {
		return new(this) String(this, src);
	}
};

#endif
