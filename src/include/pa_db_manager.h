/** @file
	Parser: sql driver manager decl.
	global sql driver manager, must be thread-safe

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_db_manager.h,v 1.14 2002/02/08 08:30:12 paf Exp $
*/

#ifndef PA_DB_MANAGER_H
#define PA_DB_MANAGER_H

#include "pa_hash.h"
#include "pa_db_connection.h"
#include "pa_cache_managers.h"

// defines

// forwards

/// sql driver manager
class DB_Manager : public Cache_manager {
	friend class DB_Connection;
public:

	DB_Manager(Pool& apool);
	virtual ~DB_Manager();

	/** 
		connect to specified db_home, 
		using driver dynamic library found in table, if not loaded yet
		checks driver version
	*/
	DB_Connection_ptr get_connection_ptr(const String& db_home, const String *source);

private: // connection cache, never expires

	DB_Connection *get_connection_from_cache(const String& db_home);
	void put_connection_to_cache(const String& db_home, DB_Connection& connection);
private:
	time_t prev_expiration_pass_time;

private:

	Hash connection_cache;

public: // Cache_manager

	virtual Value& get_status(Pool& pool, const String *source);
	virtual void maybe_expire_cache();

};

/// global
extern DB_Manager *DB_manager;

#endif
