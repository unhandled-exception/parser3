/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vint.h,v 1.11 2001/03/26 08:27:28 paf Exp $
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "_int.h"

class VInt : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "int"; }
	// int: this
	Value *get_expr_result() { return this; }

	// integer: finteger
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		snprintf(buf, MAX_NUMBER, "%d", finteger);
		return NEW String(pool(), buf);
	}
	// integer: finteger
	double get_double() { return finteger; }
	// integer: 0 or !0
	bool get_bool() { return finteger!=0; }

protected: // VAliased

	VStateless_class *get_class_alias() { return 0; }

public: // usage

	VInt(Pool& apool, int ainteger) : VStateless_object(apool, *int_class), 
		finteger(ainteger) {
	}

	int get_int() { return finteger; }
	void set_int(int ainteger) { finteger=ainteger; }

	void inc(int increment) { finteger+=increment; }
	void mul(double k) { finteger=(int)(finteger*k); }
	void div(double d) { finteger=(int)(finteger/d); }
	void mod(int d) { finteger%=d; }

private:

	int finteger;

};

#endif
