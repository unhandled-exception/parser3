/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.h,v 1.53 2001/03/14 16:47:29 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_vclass.h"
#include "pa_vobject.h"
#include "pa_venv.h"
#include "pa_vform.h"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS  \
		const char *source, \
		VStateless_class *aclass, const String *name, \
		VStateless_class *base_class, \
		const char *file
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class, file)
#else
#	define COMPILE_PARAMS  \
		const char *source, \
		VStateless_class *aclass, const String *name, \
		VStateless_class *base_class
#	define COMPILE(source, aclass, name, base_class, file)  \
		real_compile(source, aclass, name, base_class)
#endif

class Temp_lang;

class Request : public Pooled {
	friend Temp_lang;
public:
	
	struct Info {
		const char *document_root;
		const char *path_translated;
		const char *request_method;
		const char *query_string;
		const char *request_uri;
		const char *content_type;
		size_t content_length;
	};
	
	Request(Pool& apool,
		Info& ainfo,
		String::Untaint_lang alang
	);
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }

	// core request processing
	char *core(
		const char *sys_auto_path1,
		const char *sys_auto_path2);

	void execute(const Array& ops);

	VStateless_class *use_file(
		const char *file, bool fail_on_read_problem=true,
		const String *name=0, 
		VStateless_class *base_class=0); // core.C
	VStateless_class *use_buf(
		const char *source, const char *file,
		VStateless_class *aclass=0, const String *name=0, 
		VStateless_class *base_class=0); // core.C
	Value& process(
		Value& value, 
		const String *name=0,
		bool intercept_string=true); // execute.C

	void write(const String& astring) {
		wcontext->write(astring, String::Untaint_lang::NO);  // write(const) = clean
	}
	void write_no_lang(String& astring) {
		// appending, sure of clean string inside
		wcontext->write(astring, String::Untaint_lang::NO);
	}

	void write_assign_lang(Value& avalue) {
		// appending possible string, assigning untaint language
		wcontext->write(avalue, flang); 
	}
	void write_pass_lang(Value& avalue) {
		// appending possible string, passing language built into string being written
		wcontext->write(avalue, String::Untaint_lang::PASS_APPENDED); 
	}
	void write_no_lang(Value& avalue) {
		// appending sure value, no strings inside
		wcontext->write(avalue, String::Untaint_lang::NO); 
	}

	void fail_if_junction_(bool is, Value& value, const String& method_name, char *msg);

	char *relative(const char *path, const char *file);
	char *absolute(const char *name);

public:
	
	//
	Info& info;

	// default base
	VClass root_class;
	// $env:fields here
	VEnv env_class;
	// $form:elements here
	VForm form_class;

	// contexts
	Value *self, *root, *rcontext;
	WContext *wcontext;

private: // core data

	// classes
	Hash fclasses;

	// execution stack
	Stack stack;

private: // compile.C

	VStateless_class& real_compile(COMPILE_PARAMS);

private: // execute.C

	char *execute_method(Value& aself, const Method& method, bool return_cstr=true);
	char *execute_method(Value& aself, const String& method_name, bool return_cstr=true);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang flang;

private: // lang manipulation

	String::Untaint_lang set_lang(String::Untaint_lang alang) {
		String::Untaint_lang result=flang;
		flang=alang;
		return result;
	}
	void restore_lang(String::Untaint_lang alang) {
		flang=alang;
	}

private: // web

	const char *fdocument_root;
	const char *fpage_filespec;

};

class Temp_lang {
	Request& frequest;
	String::Untaint_lang saved_lang;
public:
	Temp_lang(Request& arequest, String::Untaint_lang alang) : 
		frequest(arequest),
		saved_lang(arequest.set_lang(alang)) {
	}
	~Temp_lang() { 
		frequest.restore_lang(saved_lang); 
	}
};

#endif
