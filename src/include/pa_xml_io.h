/** @file
	Parser: plugins to xml library, controlling i/o.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML
#ifndef PA_XML_IO_H
#define PA_XML_IO_H

#define IDENT_PA_XML_IO_H "$Id: pa_xml_io.h,v 1.8 2017/02/07 22:00:38 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"

void pa_xml_io_init();

typedef HashString<bool> HashStringBool;

void pa_xmlStartMonitoringDependencies();
HashStringBool* pa_xmlGetDependencies();

#endif
#endif
