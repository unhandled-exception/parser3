/** @file
	Parser: Value, Method, Junction .

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

static const char * const IDENT_VALUE_H="$Date: 2007/04/23 10:30:49 $";

#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_property.h"

// forwards

class VStateless_class;
class WContext;
class Request; class Request_charsets;
class Junction;
class VJunction;
class Method;
template<typename K, typename V> class Hash;
class Value;
typedef Hash<const String::Body, Value*> HashStringValue; 
typedef Hash<const String::Body, PA_Object*> HashStringObject;
typedef Array<Value*> ArrayValue;
class MethodParams;
class VObject;
class VMethodFrame;
class VFile;
class Table;

///	grandfather of all @a values in @b Parser
class Value: public PA_Object {
public: // Value

	/// value type, used for error reporting and 'is' expression operator
	virtual const char* type() const =0;

	/** remember derived class instance 
	    - VObject: the only client
	*/
	virtual VObject* set_derived(VObject* /*aderived*/);

	/**
		all except VObject/VClass: this if @atype eq type()
		VObject/VClass: can locate parent class by it's type
	*/
	virtual Value* as(const char* atype, bool /*looking_up*/) {
		return atype && strcmp(type(), atype)==0?this:0;
	}
	/// type checking helper, uses Value::as
	bool is(const char* atype) { return as(atype, false)!=0; }
	
	/// is this value defined?
	virtual bool is_defined() const { return true; }
	
	/// is this value string?
	virtual bool is_string() const { return false; }

	/// is this value void?
	virtual bool is_void() const { return false; }

	/// is this value bool?
	virtual bool is_bool() const { return false; }

	/// is this value number?
	virtual bool is_evaluated_expr() const { return false; }

	/// what's the meaning of this value in context of expression?
	virtual Value& as_expr_result(bool /*return_string_as_is*/=false) {
		return *bark("is '%s', can not be used in expression"); 
	}
	
	

	/** extract HashStringValue if any
		WARNING: FOR LOCAL USE ONLY, THIS POINTER IS NOT TO PASS TO ANYBODY!
	*/
	virtual HashStringValue* get_hash() { return 0; }
	
	/// extract const String
	virtual const String* get_string() { return 0; }
	
	/// extract double
	virtual double as_double() const { bark("is '%s', it does not have numerical (double) value"); return 0; }
	
	/// extract integer
	virtual int as_int () const { bark("is '%s', it does not have numerical (int) value"); return 0; }

	/// extract bool
	virtual bool as_bool() const { bark("is '%s', it does not have logical value"); return 0; }
	
	/// extract file
	virtual VFile* as_vfile(String::Language lang=String::L_UNSPECIFIED, 
		const Request_charsets* charsets=0);
	
	/// extract Junction
	virtual Junction* get_junction();
	
	/// extract Property
	virtual Property* get_property() { return 0; }
	
	/** extract base object of Value
		@return for
		- VObject: fbase
	*/
	virtual Value* base_object();
	
	/// @return Value element; can return Junction for methods; Code-Junction for code; Getter-Junction for property
	virtual Value* get_element(const String& /*aname*/, Value& /*aself*/, bool /*looking_up*/);

	/// indicator value meaning that put_element overwritten something
	#define PUT_ELEMENT_REPLACED_ELEMENT reinterpret_cast<const VJunction*>(1)
	/// store Value element under @a name
	/// @returns putter method junction, or it can just report[PUT_ELEMENT_REPLACED_ELEMENT] 
	/// that it replaced something in base fields 
	virtual const VJunction* put_element(Value& /*aself*/, const String& aname, Value* /*avalue*/, bool /*areplace*/) { 
		// to prevent modification of system classes,
		// created at system startup, and not having exception
		// handler installed, we neet to bark using request.pool
		bark("element can not be stored to %s", &aname); 
		return 0;
	}
	
	/// extract VStateless_class
	virtual VStateless_class *get_class()=0;

	/// extract VStateless_class
	virtual VStateless_class *get_last_derived_class() {
		return get_class();
	};
	/// extract base object or class of Value, if any
	virtual Value* base() { return 0; }

	/// extract VTable
	virtual Table* get_table() { return 0; }

public: // usage

	/// @return sure String. if it doesn't have string value barks
	const String& as_string() {
		const String* result=get_string(); 
		if(!result)
			bark("is '%s', it has no string representation");

		return *result;
	}

protected: 

	/// throws exception specifying bark-reason and name() type() of problematic value
	Value* bark(const char *reason, const String *problem_source=0) const {
		//if(this) // removing warnings on unreachable code
			throw Exception(PARSER_RUNTIME,
				problem_source, 
				reason, type());

		//return 0;
	}

};
	

/**
	$content-type[text/html] -> 
		content-type: text/html
	$content-type[$value[text/html] charset[windows-1251]] -> 
		content-type: text/html; charset=windows-1251
*/
const String& attributed_meaning_to_string(Value& meaning, 
					   String::Language lang, 
					   bool forced=false,
					   bool allow_bool=false);

// defines

///@{common field names
#define CHARSET_NAME "charset"
#define VALUE_NAME "value"
#define EXPIRES_NAME "expires"
#define CONTENT_TYPE_NAME "content-type"
#define NAME_NAME "name"
//@}

///@{common field names
extern const String value_name;
extern const String expires_name;
extern const String content_type_name;
extern const String name_name;
///@}

#endif
