/** @file
	Parser: @b xnode parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vxnode.h,v 1.3 2001/10/09 14:25:30 parser Exp $
*/

#ifndef PA_VXNODE_H
#define PA_VXNODE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

#include <XalanDOM/XalanNode.hpp>
#include <PlatformSupport/XSLException.hpp>

extern Methoded *Xnode_class;

//void VXnode_cleanup(void *);

/// value of type 'xnode'. implemented with XalanNode
class VXnode : public VStateless_object {
	//friend void VXnode_cleanup(void *);
public: // Value

	const char *type() const { return "xnode"; }

	/// VXnode: true
	bool as_bool() const { return true; }

	/// VXnode: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VXnode: $CLASS,$method, fields
	Value *get_element(const String& aname);

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VXnode(Pool& apool, XalanNode *anode=0, VStateless_class& abase=*Xnode_class) : 
		VStateless_object(apool, abase), 
		fnode(anode) {
		//register_cleanup(VXnode_cleanup, this);
	}
private:
	//void cleanup() {}
public:

	void set_node(XalanNode& anode) { fnode=&anode; }

public: // VXnode
	virtual XalanNode &get_node(Pool& pool, const String *source) { 
		if(!fnode)
			PTHROW(0, 0,
				source,
				"can not be applied to uninitialized instance");
		return *fnode; 
	}

private:

	XalanNode *fnode;

};

#endif
