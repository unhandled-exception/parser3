/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

static const char * const IDENT_VCODE_FRAME_H="$Date: 2009/08/08 13:30:21 $";

#include "pa_wcontext.h"
#include "pa_vvoid.h"

/// specialized write wrapper, completely transparent
class VCodeFrame: public WContext {
public: // Value

	override const char* type() const { return "code_frame"; }
	/// VCodeFrame: twice transparent
	override Value* get_element(const String& aname) { 
		return fparent->get_element(aname); 
	}
	/// VCodeFrame: twice transparent
	override const VJunction* put_element(const String& aname, Value* avalue, bool areplace) { 
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		return fparent->put_element(aname, avalue, areplace); 
	}


public: // usage

	VCodeFrame(WContext& parent): 
		WContext(&parent){
	}

};

#endif
