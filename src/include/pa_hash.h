/** @file
	Parser: hash class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_hash.h,v 1.34 2001/04/04 10:50:34 paf Exp $
*/

#ifndef PA_HASH_H
#define PA_HASH_H

#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"


/** 
	Pooled hash.

	Automatically rehashed when almost full.

	can be thread safe, wich is specified in constructor,
	default 'not safe'.
*/
class Hash : public Pooled {
public:

	typedef String Key; ///< hash Key type. longing for templates
	typedef void Val; ///< hash Val type. longing for templates

	/// for_each iterator function type
	typedef void (*For_each_func)(const Key& key, Val *value, void *info);

public:

	Hash(Pool& apool,bool athread_safe=false) : Pooled(apool) { 
		construct(apool, athread_safe); 
	}

	/// useful generic hash function
	static uint generic_code(uint aresult, const char *start, uint allocated);

	/// put a [value] under the [key], return existed or not
	/*SYNCHRONIZED*/ bool put(const Key& key, Val *value);
/*
	/// dirty hack to allow constant items storage. I long for Hash<const Val*>
	/*SYNCHRONIZED* / bool put(const Key& key, const Val *value) {
		return put(key, const_cast<Val *>(value)); 
	}
*/
	/// get associated [value] by the [key]
	/*SYNCHRONIZED*/ Val *get(const Key& key) const;

	/// put a [value] under the [key] if that [key] existed, return existed or not
	/*SYNCHRONIZED*/ bool put_replace(const Key& key, Val *value);

	/// put a [value] under the [key] if that [key] NOT existed, return existed or not
	/*SYNCHRONIZED*/ bool put_dont_replace(const Key& key, Val *value);

	/// put all 'src' values if NO with same key existed
	/*SYNCHRONIZED*/ void merge_dont_replace(const Hash& src);

	void put(const Key& key, int     value) { put(key, reinterpret_cast<Val *>(value)); }
	void put(const Key& key, String *value) { put(key, static_cast<Val *>(value)); }

	//@{
	/// handy get, longing for Hash<int>, Hash<String *>
	int get_int(const Key& key) const { return reinterpret_cast<int>(get(key)); }
	const String *get_string(const Key& key) const { return static_cast<String *>(get(key)); }
	//@}

	/// number of elements in hash
	int size() { return used; }

	/// iterate over all not zero elements
	void for_each(For_each_func func, void *info=0);

	/// remove all elements
	void clear();

protected:

	void construct(Pool& apool, bool athread_safe);

private:

	// expand when these %% of allocated exausted
	enum {
		THRESHOLD_PERCENT=75
	};

	// am I thread-safe?
	bool fthread_safe;

	// the index of [allocated] in [allocates]
	int allocates_index;

	// possible [allocates]. prime numbers
	static uint allocates[];
	static int allocates_count;

	// number of allocated pairs
	int allocated;

	// helper: expanding when used == threshold
	int threshold;

	// used pairs
	int used;

	// main storage
	class Pair {
		friend Hash;

		uint code;
		const Key key;
		Val *value;
		Pair *link;
		
		void *operator new(size_t allocated, Pool& apool);

		Pair(uint acode, const Key& akey, Val *avalue, Pair *alink) :
			code(acode),
			key(akey),
			value(avalue),
			link(alink) {}
	} **refs;

	// filled to threshold: needs expanding
	bool full() { return used==threshold; }

	// allocate larger buffer & rehash
	void expand();

private: //disabled

	//Hash(Hash&) {}
	Hash& operator = (const Hash&) { return *this; }
};

#endif
