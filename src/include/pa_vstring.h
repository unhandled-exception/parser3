/*
  $Id: pa_vstring.h,v 1.3 2001/02/22 16:21:49 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include "pa_value.h"

class VString : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "String"; }
	// string: value
	String *get_string() { return string; };
	// string: value
	//void put_string(String *astring) { string=astring; }

public: // usage

	VString(Pool& apool) : Value(apool), 
		string(new(apool) String(apool)) {
	}

	VString(String *astring) : Value(astring->pool()), 
		string(astring) {
	}

	operator String() { return *string; }

private:
	String *string;

};

#endif
