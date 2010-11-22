/** @file
	Parser: apache 1.3 and 2.2 module

	Copyright (c) 2001-2010 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_MOD_PARSER3_C="$Date: 2010/11/22 23:42:09 $";

#ifdef WIN32
#include <winsock2.h>
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"

#include "pa_httpd.h"

#define PARSER3_HANDLER "parser3-handler"

/*
* To Ease Compatibility
*/
#ifdef STANDARD20_MODULE_STUFF

#include "apr_strings.h"

#define ap_pcalloc	apr_pcalloc
#define ap_pstrdup	apr_pstrdup

#define ap_table_get	apr_table_get
#define ap_table_elts	apr_table_elts
#define ap_table_addn	apr_table_addn
#define ap_table_do	apr_table_do

#else

#include "ap_alloc.h"

#define apr_pool_t pool
#define apr_table_t table

#endif /* STANDARD20_MODULE_STUFF */

/*
* Declare ourselves so the configuration routines can find and know us.
* We'll fill it in at the end of the module.
*/

#ifdef STANDARD20_MODULE_STUFF
module AP_MODULE_DECLARE_DATA parser3_module;
#else
module MODULE_VAR_EXPORT parser3_module;
#endif

/*
* Locate our directory configuration record for the current request.
*/
static Parser_module_config *our_dconfig(request_rec *r) {
	return (Parser_module_config *) ap_get_module_config(r->per_dir_config, &parser3_module);
}

static const char* cmd_parser_config(cmd_parms *cmd, void *mconfig, char *file_spec) {
	Parser_module_config *cfg = (Parser_module_config *) mconfig;
	cfg->parser_config_filespec=file_spec;
	return NULL;
}

/* 
* Now let's declare routines for each of the callback phase in order.
*/

static int parser_handler(request_rec *r) {
#ifdef STANDARD20_MODULE_STUFF
	if(strcmp(r->handler, PARSER3_HANDLER))
		return DECLINED;
#endif
	// converting to parser version
	pa_request_rec pr={
		r,
		r->pool,
		r->header_only,
		&r->status,
		r->method,
		r->headers_out,
		r->subprocess_env,
		&r->content_type,
		r->uri,
		r->filename,
		r->path_info,
		r->args,
#ifdef STANDARD20_MODULE_STUFF
		r->finfo.filetype == 0
#else		
		r->finfo.st_mode == 0
#endif
	};
	return pa_parser_handler(&pr, our_dconfig(r));
}

/* 
* This function is called during process initialisation.
*/

#ifdef STANDARD20_MODULE_STUFF
static void parser_module_init(apr_pool_t *p, server_rec *s) {
#else
static void parser_module_init(server_rec *s, apr_pool_t *p) {
#endif	
	ap_log_perror(APLOG_MARK, APLOG_EMERG, 0, p, "parser inited %d", getpid());
	pa_setup_module_cells();
}

/* 
* All our process-death routine does is add its trace to the log.
*/
static void parser_module_done(server_rec *s, apr_pool_t *p) {
	pa_destroy_module_cells();
}

/*
* This function gets called to create a per-directory configuration record.
*/
static void *parser_create_dir_config(apr_pool_t *p, char *dirspec) {
	Parser_module_config *cfg= ap_pcalloc(p, sizeof(Parser_module_config));
	cfg->parser_config_filespec=NULL;
	return cfg;
}

/*
* This function gets called to create a per-server configuration record.
*/
static void *parser_create_server_config(apr_pool_t *p, server_rec *s) {
	Parser_module_config *cfg= ap_pcalloc(p, sizeof(Parser_module_config));
	cfg->parser_config_filespec=NULL;
	return cfg;
}

/* 
* List of directives specific to our module.
*/
static const command_rec parser_cmds[] =
{
	{
		"ParserConfig",				/* directive name */
			(const char* (*)(void))((void *)cmd_parser_config), // config action routine
			(void*)0,			/* argument to include in call */
			(int)(OR_OPTIONS),		/* where available */
			TAKE1,				/* arguments */
			"Parser config filespec"	// directive description
	},
	{NULL}
};

/*--------------------------------------------------------------------------*/
/* Now the list of content handlers available from this module.             */
/*--------------------------------------------------------------------------*/

#ifndef STANDARD20_MODULE_STUFF
static const handler_rec parser_handlers[] =
{
	{PARSER3_HANDLER, parser_handler},
	{NULL}
};
#endif

/*--------------------------------------------------------------------------*/
/* Finally, the list of callback routines and data structures that          */
/* provide the hooks into our module from the other parts of the server.    */
/*--------------------------------------------------------------------------*/

#ifdef STANDARD20_MODULE_STUFF

/* 
* register hooks.
*/
static void parser_register_hooks(apr_pool_t* pool)
{
//	ap_hook_post_config(parser_server_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(parser_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(parser_module_init, NULL, NULL, APR_HOOK_MIDDLE);
};

module AP_MODULE_DECLARE_DATA parser3_module =
{
	STANDARD20_MODULE_STUFF,
#else
module MODULE_VAR_EXPORT parser3_module =
{
	STANDARD_MODULE_STUFF,
	parser_module_init,		/* module initializer */
#endif
	parser_create_dir_config,	/* per-directory config creator */
	0,				/* dir config merger */
	parser_create_server_config,	/* server config creator */
	0,				/* server config merger */
	parser_cmds,			/* command apr_table_t */
#ifdef STANDARD20_MODULE_STUFF
	parser_register_hooks		/* register hooks */
#else
	parser_handlers,		/* [9] list of handlers */
	0,				/* [2] filename-to-URI translation */
	0,				/* [5] check/validate user_id */
	0,				/* [6] check user_id is valid *here* */
	0,				/* [4] check access by host address */
	0,				/* [7] MIME type checker/setter */
	0,				/* [8] fixups */
	0,				/* [10] logger */
	0,				/* [3] header parser */
	0,				/* process initializer */
	parser_module_done		/* process exit/cleanup */
#endif // STANDARD20_MODULE_STUFF
};

#if defined(_MSC_VER)
#	define APACHE_WIN32_SRC "../../../../win32/apache13/src"
#	ifdef _DEBUG
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreD/ApacheCore.lib")
#	else
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreR/ApacheCore.lib")
#	endif
#endif


// interface to C++

#define	PA_APLOG_EMERG		0	/* system is unusable */
#define	PA_APLOG_ALERT		1	/* action must be taken immediately */
#define	PA_APLOG_CRIT		2	/* critical conditions */
#define	PA_APLOG_ERR		3	/* error conditions */
#define	PA_APLOG_WARNING	4	/* warning conditions */
#define	PA_APLOG_NOTICE		5	/* normal but significant condition */
#define	PA_APLOG_INFO		6	/* informational */
#define	PA_APLOG_DEBUG		7	/* debug-level messages */

#define	PA_APLOG_LEVELMASK	7	/* mask off the level value */

#define PA_APLOG_NOERRNO	(PA_APLOG_LEVELMASK + 1)

#define PA_APLOG_MARK	__FILE__,__LINE__

void pa_ap_log_rerror(const char *file, int line, int level, const pa_request_rec *s, const char *fmt, ...) {
	const char* str;
	va_list l;
	va_start(l, fmt); 
	str=va_arg(l, const char*);
	va_end(l); 

	ap_log_rerror(file, line, level,
#ifdef STANDARD20_MODULE_STUFF
				0,
#endif
		      (request_rec*)s->real_request_rec, "%s", str);
}


void pa_ap_log_error(const char *file, int line, int level, const pa_server_rec *s, const char *fmt, ...) {
	const char* str;
	va_list l;
	va_start(l, fmt); 
	str=va_arg(l, const char*);
	va_end(l); 

	ap_log_error(file, line, level,
#ifdef STANDARD20_MODULE_STUFF
				0,
#endif
		      (server_rec*)s, "%s", str);
}

// ap_alloc.h

const char* pa_ap_table_get(const pa_table *t, const char *name) {
	return ap_table_get((const apr_table_t*)t, name);
}
void pa_ap_table_addn(pa_table *t, const char *name, const char *val) {
	ap_table_addn((apr_table_t*)t, name, val);
}

int pa_ap_table_size(const pa_table *t) {
	return ap_table_elts((const apr_table_t*)t)->nelts;
}

void pa_ap_table_do(int (*comp) (void *, const char *, const char *), void *rec, const pa_table *t, ...) {
	ap_table_do(comp, rec, (apr_table_t*)t, 0);
}

char * pa_ap_pstrdup(pa_pool *p, const char *s) {
	return ap_pstrdup((apr_pool_t*)p, s);
}

// http_protocol.h

int pa_ap_setup_client_block(pa_request_rec *r, int read_policy) {
	return ap_setup_client_block((request_rec*)r->real_request_rec,
		read_policy);
}
int pa_ap_should_client_block(pa_request_rec *r) {
	return ap_should_client_block((request_rec*)r->real_request_rec);
}
long pa_ap_get_client_block(pa_request_rec *r, char *buffer, int bufsiz) {
	return ap_get_client_block((request_rec*)r->real_request_rec,
		buffer, bufsiz);
}
void pa_ap_send_http_header(pa_request_rec *r) {
// Apache2 send headers before body automatically
#ifndef STANDARD20_MODULE_STUFF
	ap_send_http_header((request_rec*)r->real_request_rec);
#endif
}
int pa_ap_rwrite(const void *buf, int nbyte, pa_request_rec *r) {
	return ap_rwrite(buf, nbyte, (request_rec*)r->real_request_rec);
}

// http_main.h

void pa_ap_hard_timeout(const char *s, pa_request_rec *r) {
// Apache 2 uses non-blocking I/O
#ifndef STANDARD20_MODULE_STUFF
	ap_hard_timeout(s, (request_rec*)r->real_request_rec);
#endif
}
void pa_ap_reset_timeout(pa_request_rec *r) {
#ifndef STANDARD20_MODULE_STUFF
	ap_reset_timeout((request_rec*)r->real_request_rec);
#endif
}
void pa_ap_kill_timeout(pa_request_rec *r) {
#ifndef STANDARD20_MODULE_STUFF
	ap_kill_timeout((request_rec*)r->real_request_rec);
#endif
}

// util_script.h

void pa_ap_add_cgi_vars(pa_request_rec *r) {
	ap_add_cgi_vars((request_rec*)r->real_request_rec);
}
void pa_ap_add_common_vars(pa_request_rec *r) {
	ap_add_common_vars((request_rec*)r->real_request_rec);
}

#ifndef WIN32
// signal.h

void (*pa_signal (int sig, void (*disp)(int)))(int) {
	if(sig==PA_SIGPIPE && disp==PA_SIG_IGN)
		return signal(SIGPIPE, SIG_IGN);

	return 0;
}
#endif
