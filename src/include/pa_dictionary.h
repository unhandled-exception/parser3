/** @file
	Parser: dictionary class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_dictionary.h,v 1.6 2001/10/05 16:12:40 parser Exp $
*/

#ifndef PA_DICTIONARY_H
#define PA_DICTIONARY_H

#include "pa_config_includes.h"
#include "pa_table.h"

/// simple dictionary, speding up lookups on contained two columned table
class Dictionary : public Pooled {
public:

	/// construct wrapper, grabbing first letters of first column into @b first
	Dictionary(Table& atable, double amin_ratio=1);

	/// get max ratio
	double max_ratio() { return fmax_ratio; }

	/// find first row that contains string in first column which starts @b src
	void* first_that_starts(const char *src, size_t src_size=0) const;

	/// where this table came from, may be NULL. proxy to Table
	const String *origin_string() { return table.origin_string(); }

private:

	Table& table;
	double fmax_ratio;

private:

	int starting_line_of[0x100]; int constructor_line;

	static void add_first(Array::Item *value, void *info);
};

#endif
