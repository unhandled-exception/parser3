/*
  $Id: pa_wwrapper.h,v 1.1 2001/02/23 17:12:58 paf Exp $
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

#include "pa_wcontext.h"

class WWrapper : public WContext {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "WWrapper"; }
	// wcontext: transparent
	Value *get_element(const String& name) { return check_value()->get_element(name); }
	// wcontext: transparent
	void put_element(const String& name, Value *avalue){ 
		if(!fvalue)
			fvalue=NEW VHash(pool());
		fvalue->put_element(name, avalue); 
	}

	// wcontext: transparent
	Method *get_method(const String& name) const { return check_value()->get_method(name); }
	// wcontext: none yet | transparent
	VClass *get_class() const { return fvalue?fvalue->get_class():0; }
	// wcontext: none yet | transparent
	bool is_or_derived_from(VClass& ancestor) { return fvalue?fvalue->is_or_derived_from(ancestor):false; }

public: // usage

	WWrapper(Pool& apool, Value *avalue) : WContext(apool, avalue) {
	}
	
private:
	// raises an exception on 0 value
	Value *check_value() const {
		if(!fvalue)
			THROW(0,0,
				0,
				"accessing wcontext without value");

		return fvalue;
	}
};

#endif
