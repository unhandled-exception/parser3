/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

static const char* IDENT_VCODE_FRAME_H="$Date: 2002/08/14 14:18:29 $";

#include "pa_wcontext.h"
#include "pa_vvoid.h"

/// specialized write wrapper, completely transparent, but intercepting string writes
class VCodeFrame : public WContext {
public: // Value

	const char *type() const { return "code_frame"; }
	/// VCodeFrame: twice transparent
	Value *get_element(const String& aname, Value *aself, bool looking_up) { 
		return wcontext.get_element(aname, aself, looking_up); 
	}
	/// VCodeFrame: twice transparent
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace) { 
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		return wcontext.put_element(aname, avalue, replace); 
	}

public: // WContext

	/// VCodeFrame: intercepting string writes 
	void write(const String& string, uchar lang) {
		fstring.append(string, lang);
	}

	/** VCodeFrame: twice transparent

		if value is VString writes fstring,
		else writes Value; raises an error if already

		$hash[^if(1){$hash}]
	*/
	void write(Value& avalue, const String* aorigin) {
		if(const String *fstring=avalue.get_string())
			write(*fstring, String::UL_PASS_APPENDED);
		else
			wcontext.write(avalue, aorigin);		
	}


public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WContext(apool, &awcontext),
		wcontext(awcontext) {
	}

private:

	WContext& wcontext;

};

#endif
