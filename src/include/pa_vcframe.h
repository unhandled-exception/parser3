/*
  $Id: pa_vcframe.h,v 1.2 2001/02/25 14:23:31 paf Exp $
*/

#ifndef PA_VCFRAME_H
#define PA_VCFRAME_H

#include "pa_wwrapper.h"
#include "pa_vunknown.h"

class VCodeFrame : public WWrapper {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "CodeFrame"; }

	// codeframe: accumulated string
	String *get_string() { return string_wcontext.get_string(); };

public: // WContext

	// codeframe: intercepting string writes 
	void write(String *astring);

public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WWrapper(apool, &awcontext, awcontext.constructing()),

		string_wcontext(apool, 
			0 /* value not used, only write(string) */,
			false /* value not used, only write(string) */) {
	}

public:
	
	WContext string_wcontext;

};

#endif
