/** @file
	Parser: @b void parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vvoid.h,v 1.7 2002/02/08 07:27:55 paf Exp $
*/

#ifndef PA_VVOID_H
#define PA_VVOID_H

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded *void_class;

/// value of type 'void'. ex: usually $sjfklsjfksjdfk has this type
class VVoid : public VStateless_object {
public: // Value

	const char *type() const { return "void"; }

	/// VVoid: ""
	const String *get_string() { return NEW String(pool()); }
	/// VVoid: false
	bool as_bool() const { return false; }
	/// VVoid: false
	bool is_defined() const { return false; }
	/// VVoid: 0
	double as_double() const { return 0; }
	/// VVoid: 0
	int as_int() const { return 0; }
	/// VVoid: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }


	/// VVoid: methods
	Value *get_element(const String& name) {
		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		return NEW VVoid(pool());
	}

public: // usage

	VVoid(Pool& apool) : VStateless_object(apool, *void_class) {
	}

};

#endif
