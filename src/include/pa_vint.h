/*
  $Id: pa_vint.h,v 1.1 2001/03/09 08:19:47 paf Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "pa_value.h"
#include "pa_common.h"
#include "classes/_int.h"

#define MAX_INT_AS_STRING 20

class VInt : public VObject {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "integer"; }
	// int: this
	Value *get_expr_result() { return this; }

	// integer: finteger
	String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_INT_AS_STRING));
		snprintf(buf, MAX_INT_AS_STRING, "%d", finteger);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	}
	// integer: finteger
	double get_double() { return finteger; }
	// integer: 0 or !0
	bool get_bool() { return finteger!=0; }

public: // usage

	VInt(Pool& apool, int ainteger) : VObject(apool, *int_class), 
		finteger(ainteger) {
	}

	int get_int() { return finteger; }

	void inc(int increment) { finteger+=increment; }

private:

	int finteger;

};

#endif
