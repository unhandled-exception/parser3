/** @image
	Parser: @b image parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vimage.h,v 1.5 2001/04/11 17:00:58 paf Exp $
*/

#ifndef PA_VIMAGE_H
#define PA_VIMAGE_H

#include "gif.h"

#include "pa_common.h"
//#include "pa_globals.h"
#include "pa_vstateless_object.h"
#include "_image.h"

/** holds <img> attributes 
	and [image itself]
*/
class VImage : public VStateless_object {
public: // Value
	
	/// all: for error reporting after fail(), etc
	const char *type() const { return "image"; }

	/// VImage: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VImage: true
	bool as_bool() { return true; }

	/// VImage: method,field
	Value *VImage::get_element(const String& aname) {
		// $method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		// $src, $size
		return static_cast<Value *>(ffields.get(aname));
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VImage::VImage(Pool& apool) : VStateless_object(apool, *image_class),
		ffields(apool) {
	}

	void set(const String *src, int width, int height,
		gdImagePtr aimage=0);
	void save(const String& image_spec) {
		/*if(fvalue_ptr)
			image_write(pool(), image_spec, fvalue_ptr, fvalue_size, false);
		else
			THROW(0, 0,
				&image_spec,
				"saving unassigned image"); //never*/
	}

	Hash fields() const { return ffields; }

public:

	gdImagePtr image;

private:

	Hash ffields;

};

#endif
