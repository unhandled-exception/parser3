/*
  $Id: pa_value.h,v 1.8 2001/02/15 14:18:49 paf Exp $
*/

/*
	data core
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_string.h"

class Method {
public:
	Array param_names;
	Array local_names;
	String code;
};

class Operator : public Method {
	// operator module static vars stored in there
	Operator_class *self;
};

class Method_ref {
public:
	Value *self;
	Method& method;
};

class Junction {
	Value *self;
	String code;
};

class Value {
public:
	// text: value
	virtual String *get_string() const =0;
	virtual String *put_string(const String *astring)=0;

	// method_ref: self, method
	virtual Method_ref *get_method_ref() const =0;

	// junction: self, code
	virtual Junction *get_junction() const =0;

	// hash: (key)=value
	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	// object_instance: (field)=value;(STATIC)=hash;(method)=method_ref
	// operator_class: (field)=value - static values only
	virtual Value *get_element(const String& name) const =0;
	virtual Value *put_element(const String& name, const Value *avalue)=0;

	// object_instance, object_class: method
	virtual Method *get_method(const String& name) const =0;

	// object_class, object_instance: object_class
	virtual Class *get_class() const =0;

	// object_class: true when this class is derived from 'parent'
	virtual bool has_parent(Class *aparent) const =0;
};

/*
descendants:
	text:+ value:String
    hash:+ keys&values:Hash
    table:+ columns_order:Array, columns:Hash, rows:Array
    object_class:+ STATIC:Hash, methods:Hash
    object_instance:+ object_class, fields:Hash
    method_ref:+ self:Value/object_class, method:String
    method_self_n_params_n_locals:+ self:Value/object_class[1st try], params_locals&values:Hash[2nd try]
	junction:+ self:Value, code:String
*/

#endif
