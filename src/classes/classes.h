/** @file
	Parser: @b Methoded class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: classes.h,v 1.8 2001/05/17 18:26:22 parser Exp $
*/

#ifndef CLASSES_H
#define CLASSES_H

#include "pa_vstateless_class.h"
#include "pa_array.h"

/**	Pure virtual base for configurable Methoded descendants
	@see Methoded_array
*/
class Methoded : public VStateless_class {
public: // Methoded

	/** should Methoded_array::register_directly_used register this class in
		Request::classes() or not. 
		if not - global variable with Methoded descendant 
		is used in VStateless_class parameter
	*/
	virtual bool used_directly() =0;
	/// use this method to read parameters from root "auto.p"; access r.main_class
	virtual void configure_admin(Request& r) {}
	/// use this method to read parameters from 'MAIN' class; access r.main_class
	virtual void configure_user(Request& r) {}

public: // usage

	Methoded(Pool& pool) : VStateless_class(pool) {
	}

	void register_directly_used(Request& r);

};

/// all Methoded registered here in autogenerated classes.C
class Methoded_array : public Array {
public:
	Methoded_array(Pool& pool);

public: // Methoded for_each-es
	/// @see Methoded::configure_admin
	void configure_admin(Request& r);
	/// @see Methoded::configure_user
	void configure_user(Request& r);
	/// @see Methoded::register_directly_used
	void register_directly_used(Request& r);
};

extern Methoded_array *methoded_array;
void init_methoded_array(Pool& pool);

#endif
