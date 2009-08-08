/** @file
	Parser: Method class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_METHOD_H
#define PA_METHOD_H

static const char * const IDENT_METHOD_H="$Date: 2009/08/08 13:30:21 $";

#define OPTIMIZE_CALL
#define OPTIMIZE_RESULT

#include "pa_operation.h"

/**
	native code method
	params can be NULL when 
	method min&max params (see VStateless_class::add_native_method)
	counts are zero.	
*/
typedef void (*NativeCodePtr)(Request& request, MethodParams& params);


/** 
	class method.

	methods can have 
	- named or
	- numbered parameters

	methods can be
	- parser or 
	- native onces

	holds
	- parameter names or number limits
	- local names
	- code [parser or native]
*/
class Method: public PA_Object {
public:

	/// allowed method call types
	enum Call_type {
		CT_ANY, ///< method can be called either statically or dynamically
		CT_STATIC, ///< method can be called only statically
		CT_DYNAMIC ///< method can be called only dynamically
	};
	
	enum Result_optimization {
		RO_UNKNOWN,
		RO_USE_RESULT, // write to $result detected, will not collect all writes to output scope.
		RO_USE_WCONTEXT // native code or parser code without $result usage.
	};

	enum Call_optimization {
		CO_NONE,
		CO_WITHOUT_FRAME, // for some native methods method frame is not required, faster
		CO_WITHOUT_WCONTEXT // for some native methods wcontext is not required, faster
	};

	///
	Call_type call_type;
	//@{
	/// @name either numbered params // for native-code methods = operators
	int min_numbered_params_count, max_numbered_params_count;
	//@}
	//@{
	/// @name or named params&locals // for parser-code methods
	ArrayString* params_names;  ArrayString* locals_names;
	//@}
	//@{
	/// @name the Code
	ArrayOperation* parser_code;/*OR*/NativeCodePtr native_code;
	//@}

	VJunction *junction_template;

	bool all_vars_local; // in local vars list 'locals' was specified: all vars are local
	bool allways_use_result; // write to $result detected. will not collect all writes to output scope.
#ifdef OPTIMIZE_RESULT
	Result_optimization result_optimization;
#endif

#ifdef OPTIMIZE_CALL
	Call_optimization call_optimization;
#endif

	Method(
		Call_type acall_type,
		int amin_numbered_params_count, int amax_numbered_params_count,
		ArrayString* aparams_names, ArrayString* alocals_names,
		ArrayOperation* aparser_code, NativeCodePtr anative_code,
		bool aall_vars_local=false
#ifdef OPTIMIZE_RESULT
		, Result_optimization aresult_optimization=RO_UNKNOWN
#endif
#ifdef OPTIMIZE_CALL
		, Call_optimization acall_optimization=CO_NONE
#endif
		) :

		call_type(acall_type),
		min_numbered_params_count(amin_numbered_params_count),
		max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code),
#ifdef OPTIMIZE_RESULT
		result_optimization(aresult_optimization),
#endif
#ifdef OPTIMIZE_CALL
		call_optimization(acall_optimization),  
#endif
		all_vars_local(aall_vars_local){
	}

	/// call this before invoking to ensure proper actual numbered params count
	void check_actual_numbered_params(
		Value& self, MethodParams* actual_numbered_params) const;
};

///	Auto-object used for temporarily substituting/removing elements
class Temp_value_element {
	Value& fwhere;
	const String& fname;
	Value* saved;
public:
	Temp_value_element(Value& awhere, const String& aname, Value* awhat) : 
		fwhere(awhere),
		fname(aname),
		saved(awhere.get_element(aname)) {
		fwhere.put_element(aname, awhat, false);
	}
	~Temp_value_element() { 
		fwhere.put_element(fname, saved, false);
	}
};


#endif
