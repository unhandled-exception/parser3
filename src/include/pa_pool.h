/** @file
	Parser: pool class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.h,v 1.59 2001/09/21 14:46:09 parser Exp $
*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include "pa_config_includes.h"

#ifdef XML
#include <XalanDOM/XalanDOMString.hpp>
#include <util/TransService.hpp>
#include <PlatformSupport/XSLException.hpp>
#endif

// forwards

class Exception;
class Temp_exception;
class String;

/** 
	Pool mechanizm allows users not to free up allocated memory,
	leaving that problem to 'pools'.

	Also holds Exception object, which can be temporary set using 
	Temp_exception auto-object.

	@see Pooled
*/

class Pool {
	friend Temp_exception;
public:

	Pool(void *astorage);
	~Pool();

	void set_context(void *acontext) { fcontext=acontext; }
	void *context() { return fcontext; }

	void set_tag(void *atag) { ftag=atag; }
	void *tag() { return ftag; }

	/// allocates some bytes on pool
	void *malloc(size_t size/*, int place=0*/) {
		return check(real_malloc(size/*, place*/), size);
	}
	/// allocates some bytes clearing them with zeros
	void *calloc(size_t size) {
		return check(real_calloc(size), size);
	}

	/// registers a routine to clean up non-pooled allocations
	void register_cleanup(void (*cleanup) (void *), void *data) {
		if(!real_register_cleanup(cleanup, data))
			fail_register_cleanup();
	}

	/// current exception object of the pool
	Exception& exception() const { return *fexception; }

private:

	void *fstorage;
	void *fcontext;
	void *ftag;

private: 
	
	//{
	/// @name implementation defined
    void *real_malloc(size_t size/*, int place*/);
    void *real_calloc(size_t size);
	bool real_register_cleanup(void (*cleanup) (void *), void *data);
	//}

private: 

	/// checks whether mem allocated OK. throws exception otherwise
	void *check(void *ptr, size_t size) {
		if(ptr)
			return ptr;

		fail_alloc(size);

		// never reached
		return 0;
	}
	/// throws allocation exception
	void fail_alloc(size_t size) const;

	/// throws register cleanup exception
	void fail_register_cleanup() const;

private: // exception handling

	// exception replacement mechanism is 'private'zed from direct usage
	// Temp_exception object enforces paired set/restore
	Exception *set_exception(Exception *e){
		Exception *r=fexception;
		fexception=e;
		return r;
	}
	void restore_exception(Exception *e) {
		fexception=e;
	}

private:

	// current request's exception object
	Exception *fexception;

#ifdef XML

public:
	/// resets transcoder if they change charset 
	void set_charset(const String &charset);
	/// returns current charset
	const String& get_charset() { return *charset; }
	/// converts Xalan string to char *
	const char *transcode_cstr(const XalanDOMString& s);
	/// converts Xalan string to parser String
	String& transcode(const XalanDOMString& s);
	/// converts XSL exception to parser exception
	void _throw(const String *source, const XSLException& e);

private:

	void set_charset(const char *new_scharset);
	void update_transcoder();

private:

	const String *charset;
	XMLTranscoder *transcoder;

#endif

private: //disabled

	// Pool(const Pool&) {}
	Pool& operator = (const Pool&) { return *this; }
};

/** 
	Base for all classes that are allocated in 'pools'.

	Holds Pool object. Contains useful wrappers to it's methods.

	@see NEW, Temp_exception
*/
class Pooled {
	// the pool i'm allocated on
	Pool *fpool;
public:

	/// the Pooled-sole: Pooled instances can be allocated in Pool rather then on heap
	static void *operator new(size_t size, Pool& apool) { 
		return apool.malloc(size/*, 1*/);
	}

	Pooled(Pool& apool) : fpool(&apool) {}

	/// my pool
	Pool& pool() const { return *fpool; }

	/** used for moving objects from one pool to another. 
		in between object can have no pool and can not be used
		@see SQL_Driver_manager
	*/
	void set_pool(Pool *apool) { fpool=apool; }

	//{
	/// @name useful wrapper around pool
	void *malloc(size_t size) const { return fpool->malloc(size); }
	void *calloc(size_t size) const { return fpool->calloc(size); }
	void register_cleanup(void (*cleanup) (void *), void *data) { fpool->register_cleanup(cleanup, data); }
	Exception& exception() const { return fpool->exception(); }
#ifdef XML
	const char *transcode_cstr(const XalanDOMString& s) { return fpool->transcode_cstr(s); }
	String& transcode(const XalanDOMString& s) { return fpool->transcode(s); }
	void _throw(const String *source, const XSLException& e) { fpool->_throw(source, e); }
#endif
	//}
};
/// useful macro for creating objects on current Pooled object Pooled::pool()
#define NEW new(pool())

/** 
	Auto-object used for temporary changing Pool's exception().

	Use by with these macros:
	@code
		TRY { 
			// ... 
			if(?) 
				THROW(?); 
			// ...
		} CATCH(e) { 
			// code, using e fields
			// e.comment() 
		}
		END_CATCH
	@endcode

	@see TRY, THROW
*/
class Temp_exception {
	Pool& fpool;
	Exception *saved_exception;
public:
	Temp_exception(Pool& apool, Exception& exception) : 
		fpool(apool),
		saved_exception(apool.set_exception(&exception)) {
	}
	~Temp_exception() { 
		fpool.restore_exception(saved_exception); 
	}
};

#define XTRY(pool) \
	{ \
		Exception temp_exception; \
		Temp_exception le(pool, temp_exception); \
		if(setjmp(temp_exception.mark)==0)

#define XTHROW(exception) exception._throw
#define XCATCH(e) \
		else{ \
			Exception& e=temp_exception;

#define XEND_CATCH \
		} \
	}

//@{
/// @see Temp_exception 
#define TRY XTRY(pool())
#define THROW XTHROW(exception())
#define CATCH(e) XCATCH(e)
#define END_CATCH XEND_CATCH 

#define PTRY XTRY(pool)
#define PTHROW XTHROW(pool.exception())
#define PCATCH(e) XCATCH(e)
#define PEND_CATCH XEND_CATCH 
//@}

#endif
