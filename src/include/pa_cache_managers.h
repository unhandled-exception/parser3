/** @file
	Parser: cache managers center decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_cache_managers.h,v 1.5 2002/02/08 07:27:42 paf Exp $
*/

#ifndef PA_CACHE_MANAGERS_H
#define PA_CACHE_MANAGERS_H

#include "pa_hash.h"
#include "pa_value.h"

// defines

// forwards

/// maintains name=>Cache_manager association, can expire its contents
class Cache_managers : public Hash {
public:
	Cache_managers(Pool& apool) : Hash(apool) {
	}

	~Cache_managers();

	/// maybe-expires all caches it contains, each cache manager desides it itself
	void maybe_expire();

};

/// can return status and can expire it contents
class Cache_manager : public Pooled {
public:
	Cache_manager(Pool& apool) : Pooled(apool) {}
	virtual ~Cache_manager() {}

	/// if filter_server_id not null, returns status only Cachable -s with matching cacheable_item_server_id()
	virtual Value& get_status(Pool& pool, const String *source) =0;
	virtual void maybe_expire_cache() {}
};

/// global
extern Cache_managers *cache_managers;

#endif
