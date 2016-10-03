/** @file
	Parser: request class decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#define IDENT_PA_REQUEST_H "$Id: pa_request.h,v 1.229 2016/10/03 20:34:48 moko Exp $"

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_request_info.h"
#include "pa_request_charsets.h"
#include "pa_sapi.h"

// consts

const size_t pseudo_file_no__process=1;

// forwards

class Temp_lang;
class Methoded;
class VMethodFrame;
class VMail;
class VForm;
class VResponse;
class VCookie;
class VStateless_class;
class VConsole;

extern int pa_execute_recoursion_limit;
extern int pa_loop_limit;

/// Main workhorse.
class Request: public PA_Object {
	friend class Temp_lang;
	friend class Temp_connection;
	friend class Temp_request_self;
	friend class Temp_value_element;
	friend class Request_context_saver;
	friend class Exception_trace;

public:
	class Trace {
		const String* fname;
		Operation::Origin forigin;
	public:
		Trace(): fname(0) {}
		void clear() { fname=0; }

		Trace(const String* aname, const Operation::Origin aorigin):
			fname(aname), forigin(aorigin) {}

		const String* name() const { return fname; }
		const Operation::Origin origin() const { return forigin; }
	};

	enum Skip {
		SKIP_NOTHING,
		SKIP_BREAK,
		SKIP_CONTINUE
	};

private:
	Pool fpool;
public:
	Pool& pool() { return fpool; }

private:
	union StackItem {
		Value* fvalue;
		ArrayOperation* fops;
		VMethodFrame* fmethod_frame;
	public:
		Value& value() const { return *fvalue; }
		const String& string() const { 
			return fvalue->as_string();
		}
		ArrayOperation& ops() const { return *fops; }
		VMethodFrame& method_frame() const { return *fmethod_frame; }

		/// needed to fill unused Array entries
		StackItem() {}
		StackItem(Value& avalue): fvalue(&avalue) {}
		StackItem(ArrayOperation& aops): fops(&aops) {}
		StackItem(VMethodFrame& amethod_frame): fmethod_frame(&amethod_frame) {}
	};

	class Exception_trace: public Stack<Trace> {
		size_t fbottom;
	public:
		Exception_trace(): fbottom(0) {}

		size_t bottom_index() { return fbottom; }
		void set_bottom_index(size_t abottom) { fbottom=abottom; }
		element_type bottom_value() { return get(bottom_index()); }

		void clear() {
			fused=fbottom=0;
		}

		bool is_empty() {
			return fused==fbottom;
		}
	};

	///@{ core data

	/// classes
	HashString<VStateless_class*> fclasses;

	/// already used files to avoid cyclic uses
	HashString<bool> used_files;
	HashString<bool> searched_along_class_path;
	/// list of all used files, Operation::file_no = index to it
	Array<String::Body> file_list;

	/// endless execute(execute(... preventing counter
	uint anti_endless_execute_recoursion;

	///@}

	/// execution stack
	Stack<StackItem> stack;

	/// exception stack trace
	Exception_trace exception_trace;
public:

	bool allow_class_replace;

	//@{ request processing status
	/// contexts
	VMethodFrame* method_frame;
	Value* rcontext;
	WContext* wcontext;
	/// current language
	String::Language flang;
	/// current connection
	SQL_Connection* fconnection;
	//@}
	/// interrupted flag, raised on signals [SIGPIPE]
	bool finterrupted;
	Skip fskip;
	int fin_cycle;

public:
	uint register_file(String::Body file_spec);

	struct Exception_details {
		const Operation::Origin origin;
		const String* problem_source;
		VHash& vhash;
		Exception_details(const Operation::Origin aorigin, const String* aproblem_source, VHash& avhash): origin(aorigin), problem_source(aproblem_source), vhash(avhash) {}
	};
	Exception_details get_details(const Exception& e);
	const char* get_exception_cstr(const Exception& e, Exception_details& details);

	/// @see Stack::wipe_unused
	void wipe_unused_execution_stack() {
		stack.wipe_unused();
	}

#ifdef RESOURCES_DEBUG
	/// measures
	double sql_connect_time;
	double sql_request_time;
#endif	

	Request(SAPI_Info& asapi_info, Request_info& arequest_info,
		String::Language adefault_lang ///< all tainted data default untainting lang
	);
	~Request();

	/// global classes
	HashString<VStateless_class*>& classes() { return fclasses; }
	VStateless_class* get_class(const String& name);
	void put_class(VStateless_class *aclass){ classes().put(aclass->type(), aclass); }

	/**
		core request processing

		BEWARE: may throw exception to you: catch it!
	*/
	void core(
		const char* config_filespec, ///< system config filespec
		bool config_fail_on_read_problem, ///< fail if system config file not found
		bool header_only);

	/// executes ops
	void execute(ArrayOperation& ops); // execute.C
	void op_call(VMethodFrame &frame);
	void op_call_write(VMethodFrame &frame);
	Value& construct(VStateless_class &class_value, const Method &method);

	/// execute ops with anti-recoursion check
	void recoursion_checked_execute(ArrayOperation& ops) {
		if(++anti_endless_execute_recoursion==pa_execute_recoursion_limit) {
			anti_endless_execute_recoursion=0; // give @exception a chance
			throw Exception(PARSER_RUNTIME, 0, "call canceled - endless recursion detected");
		}
		execute(ops); // execute it
		anti_endless_execute_recoursion--;
	}

	///
	void use_file_directly(VStateless_class& aclass, const String& file_spec, bool fail_on_read_problem=true, bool fail_on_file_absence=true);

	/// compiles the file, maybe forcing it's class @a name and @a base_class.
	void use_file(VStateless_class& aclass, const String& file_name, const String* use_filespec);
	void use_file(VStateless_class& aclass, const String& file_name, const String* use_filespec, Operation::Origin origin);

	/// compiles a @a source buffer
	void use_buf(VStateless_class& aclass, const char* source, const String* main_alias, uint file_no, int line_no_offset=0);

	/// processes any code-junction there may be inside of @a value
	Value& process_getter(Junction& junction); // execute.C
	Value& process(Value& input_value, bool intercept_string=true); // execute.C
	void process_write(Value& input_value); // execute.C

	//@{ convinient helpers
	const String& process_to_string(Value& input_value) {
		return process(input_value, true/*intercept_string*/).as_string();
	}
	Value& process_to_value(Value& input_value, bool intercept_string=true) {
		return process(input_value, intercept_string);
	}
	//@}
	const String* get_method_filename(const Method* method); // execute.C
	const String* get_used_filename(uint file_no);
	
	/// appending, sure of clean string inside
	void write_no_lang(const String& astring) {
		wcontext->write(astring, 
			(String::Language)(String::L_CLEAN | (flang & String::L_OPTIMIZE_BIT) ));
	}
	/// appending sure value, that would be converted to clean string
	void write_no_lang(Value& avalue) {
		if(wcontext->get_in_expression())
			wcontext->write(avalue);
		else
			wcontext->write(avalue, (String::Language)(String::L_CLEAN | (flang & String::L_OPTIMIZE_BIT) ));
	}

	/// appending string, passing language built into string being written
	void write_pass_lang(const String& astring) {
		wcontext->write(astring, String::L_PASS_APPENDED);
	}
	/// appending possible string, passing language built into string being written
	void write_pass_lang(Value& avalue) {
		wcontext->write(avalue, String::L_PASS_APPENDED);
	}

	/// appending possible string, assigning untaint language
	void write_assign_lang(Value& avalue) {
		wcontext->write(avalue, flang); 
	}
	/// appending string, assigning untaint language
	void write_assign_lang(const String& astring) {
		wcontext->write(astring, flang); 
	}

	/// appending sure value
	void write_value(Value& avalue) {
		wcontext->write(avalue);
	}

	/// returns relative to @a path  path to @a file 
	const String& relative(const char* apath, const String& relative_name);

	/// returns an absolute @a path to relative @a name
	const String& absolute(const String& relative_name);

	/// returns the mime type of 'user_file_name'
	const String& mime_type_of(const String* file_name);

	/// returns the mime type of 'user_file_name_cstr'
	const String& mime_type_of(const char* user_file_name_cstr);

	/// returns current SQL connection if any
	SQL_Connection* connection(bool fail_on_error=true) {
		if(fail_on_error && !fconnection)
			throw Exception(PARSER_RUNTIME, 0, "outside of 'connect' operator");
		return fconnection;
	}

	void set_interrupted(bool ainterrupted) { finterrupted=ainterrupted; }
	bool get_interrupted() { return finterrupted; }

	void set_skip(Skip askip) { fskip=askip; }
	Skip get_skip() { return fskip; }

	void set_in_cycle(int adelta) { fin_cycle+=adelta; }
	bool get_in_cycle() { return fin_cycle>0; }

public:
	
	/// info from web server
	Request_info& request_info;

	/// info about ServerAPI
	SAPI_Info& sapi_info;

	/// source, client, mail charsets
	Request_charsets charsets;

	/// 'MAIN' class conglomerat & operators are methods of this class
	VStateless_class& main_class;
	/// $form:elements
	VForm& form;
	/// $mail
	VMail& mail;
	/// $response:elements
	VResponse& response;
	/// $cookie:elements
	VCookie& cookie;
	/// $console
	VConsole& console;

	/// classes configured data
	HashString<void*> classes_conf;

public: // status read methods

	VMethodFrame *get_method_frame() { return method_frame; }
	Value& get_self();

#define GET_SELF(request, type) (static_cast<type &>(request.get_self()))

	/// public for ^reflection:copy[]
	void put_element(Value& ncontext, const String& name, Value* value);

	/// for @main[]
	const String* execute_virtual_method(Value& aself, const String& method_name);

	/// executes parser method, use op_call(frame) to execute native method
	void execute_method(VMethodFrame& aframe);

	//{ for @conf[filespec] and @auto[filespec] and parser://method/call
	const String* execute_method(Value& aself, const Method& method, Value* optional_param, bool do_return_string);

	struct Execute_nonvirtual_method_result {
		const String* string;
		Method* method;
		Execute_nonvirtual_method_result(): string(0), method(0) {}
	};

	Execute_nonvirtual_method_result execute_nonvirtual_method(VStateless_class& aclass, const String& method_name, VString* optional_param, bool do_return_string);
	//}

#ifdef XML
public: // charset helpers

	/// @see Charset::transcode
	xmlChar* transcode(const String& s);
	/// @see Charset::transcode
	xmlChar* transcode(const String::Body s);
	/// @see Charset::transcode
	const String& transcode(const xmlChar* s);

#endif

private:

	/// already executed some @conf method
	bool configure_admin_done;

	void configure_admin(VStateless_class& conf_class);

	void configure();

private: // compile.C

	ArrayClass& compile(VStateless_class* aclass, const char* source, const String* main_alias, uint file_no, int line_no_offset);

private: // execute.C

	Value& get_element(Value& ncontext, const String& name);
#ifdef FEATURE_GET_ELEMENT4CALL
	Value& get_element4call(Value& ncontext, const String& name);
#endif

private: // defaults

	const String::Language fdefault_lang;

private: // mime types

	/// $MAIN:MIME-TYPES
	Table *mime_types;

private: // lang manipulation

	String::Language set_lang(String::Language alang) {
		String::Language result=flang;
		flang=alang;
		return result;
	}
	void restore_lang(String::Language alang) {
		flang=alang;
	}

private: // connection manipulation

	SQL_Connection* set_connection(SQL_Connection* aconnection) {
		SQL_Connection* result=fconnection;
		fconnection=aconnection;
		return result;
	}
	void restore_connection(SQL_Connection* aconnection) {
		fconnection=aconnection;
	}

private:

	void output_result(VFile* body_file, bool header_only, bool as_attachment);
};

/// Auto-object used to save request context across ^try body
class Request_context_saver {
	Request& fr;

	/// exception stack trace
	size_t exception_trace_top;
	size_t exception_trace_bottom;
	/// execution stack
	size_t stack;
	uint anti_endless_execute_recoursion;
	/// contexts
	VMethodFrame* method_frame;
	Value* rcontext;
	WContext* wcontext;
	/// current language
	String::Language flang; 
	/// current connection
	SQL_Connection* fconnection;

public:
	Request_context_saver(Request& ar) :
		fr(ar),
		exception_trace_top(ar.exception_trace.top_index()),
		exception_trace_bottom(ar.exception_trace.bottom_index()),
		stack(ar.stack.top_index()),
		anti_endless_execute_recoursion(ar.anti_endless_execute_recoursion),
		method_frame(ar.method_frame),
		rcontext(ar.rcontext),
		wcontext(ar.wcontext),
		flang(ar.flang),
		fconnection(ar.fconnection) {}
	void restore() {
		fr.exception_trace.set_top_index(exception_trace_top);
		fr.exception_trace.set_bottom_index(exception_trace_bottom);
		fr.stack.set_top_index(stack);
		fr.anti_endless_execute_recoursion=anti_endless_execute_recoursion;
		fr.method_frame=method_frame, fr.rcontext=rcontext; fr.wcontext=wcontext;
		fr.flang=flang;
		fr.fconnection=fconnection;
	}
};

///	Auto-object used for temporary changing Request::flang.
class Temp_lang {
	Request& frequest;
	String::Language saved_lang;
public:
	Temp_lang(Request& arequest, String::Language alang) :
		frequest(arequest),
		saved_lang(arequest.set_lang(alang)) {
	}
	~Temp_lang() {
		frequest.restore_lang(saved_lang);
	}
};

///	Auto-object used for temporary changing Request::fconnection.
class Temp_connection {
	Request& frequest;
	SQL_Connection* saved_connection;
public:
	Temp_connection(Request& arequest, SQL_Connection* aconnection) :
		frequest(arequest),
		saved_connection(arequest.set_connection(aconnection)) {
	}
	~Temp_connection() {
		frequest.restore_connection(saved_connection); 
	}
};

///	Auto-object used for break out of cycle check
class InCycle {
	Request& frequest;
public:
	InCycle(Request& arequest) : frequest(arequest) {
		frequest.set_in_cycle(1);
	}
	~InCycle() {
		frequest.set_in_cycle(-1);
	}
};

///	Auto-object used for temporary changing Request::allow_class_replace.
class Temp_class_replace {
	Request& frequest;
public:
	Temp_class_replace(Request& arequest, bool avalue) : frequest(arequest){
		frequest.allow_class_replace=avalue;
	}
	~Temp_class_replace() {
		frequest.allow_class_replace=false;
	}
};

///	Auto-object used for temporarily substituting/removing elements
class Temp_value_element {
	Request& frequest;
	Value& fwhere;
	const String& fname;
	Value* saved;
public:
	Temp_value_element(Request& arequest, Value& awhere, const String& aname, Value* awhat);
	~Temp_value_element();
};

// defines for externs

#define EXCEPTION_HANDLED_PART_NAME "handled"


// externs

extern const String main_method_name;
extern const String auto_method_name;

extern const String exception_type_part_name;
extern const String exception_source_part_name;
extern const String exception_comment_part_name;
extern const String exception_handled_part_name;

// defines for statics

#define MAIN_CLASS_NAME "MAIN"
#define AUTO_FILE_NAME "auto.p"

#endif
