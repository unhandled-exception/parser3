/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_vclass.h,v 1.29 2001/03/10 16:34:36 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_valiased.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

class VClass : public VAliased {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& aname);

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value);

	// object_class, object_instance: object_class
	VClass *get_class() { return this; }

public: // usage

	VClass(Pool& apool) : VAliased(apool, *this), 
		fbase(0),
		ffields(apool),
		fmethods(apool) {
	}

	void add_method(const String& name, Method& method) {
		fmethods.put(name, &method);
	}
//	Hash& methods() { return fmethods; }
	
	void set_base(VClass& abase) {
		// remember the guy
		fbase=&abase;
	}
	VClass *base() { return fbase; }

	bool is_or_derived_from(VClass& vclass) {
		return 
			this==&vclass || 
			fbase && fbase->is_or_derived_from(vclass);
	}

	Junction *get_junction(VAliased& self, const String& name) {
		if(Method *method=static_cast<Method *>(fmethods.get(name)))
			return NEW Junction(pool(), self, this, method, 0,0,0,0);
		if(fbase)
			return fbase->get_junction(self, name);
		return 0; 
	}

	void set_field(const String& name, Value *value) {
		value->set_name(name);
		if(fbase && fbase->replace_field(name, value))
			return;

		ffields.put(name, value);
	}

private:

	Value *get_field(const String& name) {
		Value *result=static_cast<Value *>(ffields.get(name));
		if(!result && fbase)
			result=fbase->get_field(name);
		return result;
	}
		
	bool replace_field(const String& name, Value *value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			ffields.put_replace(name, value);
	}
	
private:

	VClass *fbase;
	Hash ffields;
	Hash fmethods;
};

#endif
