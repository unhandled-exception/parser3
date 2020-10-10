#ifndef PA_SAPI_INFO_H
#define PA_SAPI_INFO_H

#define IDENT_PA_SAPI_INFO_H "$Id: pa_sapi_info.h,v 1.1 2020/10/10 06:08:37 moko Exp $"

#include "pa_sapi.h"
#include "pa_http.h"

/// IIS refuses to read bigger chunks
const size_t READ_POST_CHUNK_SIZE=0x400*0x400; // 1M

class SAPI_Info : public PA_Allocated {
public:
	int http_response_code;

	SAPI_Info() : http_response_code(0) {}

	virtual char* get_env(const char* name) {
		if(char *local=getenv(name))
			return pa_strdup(local);
		else
			return 0;
	}

	virtual const char* const *get_env() {
#ifdef _MSC_VER
		extern char **_environ;
		return _environ;
#else
		extern char **environ;
		return environ;
#endif
	}

	virtual size_t read_post(char *, size_t) {
		return 0;
	}

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
	}

	virtual void send_header() {}

	virtual size_t send_body(const void *buf, size_t size) {
		return stdout_write(buf, size);
	}

	virtual void die(const char *content, int content_length) {
		stdout_write(content, content_length);
	}

} *sapiInfo = NULL;

class SAPI_Info_CGI : public SAPI_Info {
public:

	virtual size_t read_post(char *buf, size_t max_bytes) {
		size_t read_size=0;
		do {
			ssize_t chunk_size=read(fileno(stdin), buf+read_size, min(READ_POST_CHUNK_SIZE, max_bytes-read_size));
			if(chunk_size<=0)
			break;
			read_size+=chunk_size;
		} while(read_size<max_bytes);
		return read_size;
	}

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		SAPI_Info::add_header_attribute(dont_store_key, dont_store_value);
//		if(!request || !request->console.was_used())
			printf("%s: %s\n", capitalize(dont_store_key), dont_store_value);
	}

	virtual void send_header() {
		puts("");
	}

	virtual void die(const char *content, int content_length) {
		// prepare header
		// let's be honest, that's bad we couldn't produce valid output
		// capitalized headers passed for preventing malloc during capitalization
		add_header_attribute(HTTP_STATUS_CAPITALIZED, "500");
		add_header_attribute(HTTP_CONTENT_TYPE_CAPITALIZED, "text/plain");
		// don't use 'format' function because it calls malloc
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, sizeof(content_length_cstr), "%u", content_length);
		add_header_attribute(HTTP_CONTENT_LENGTH_CAPITALIZED, content_length_cstr);

		// send header
		send_header();
		// body
		send_body(content, content_length);
	}

};

class SAPI_Info_HTTPD : public SAPI_Info {
public:

	HTTPD_Connection &connection;
	String output;

	SAPI_Info_HTTPD(HTTPD_Connection &aconnection) : connection(aconnection) {}

	virtual char* get_env(const char* name) {
		for(Array_iterator<ResponseHeaders::Header> i(connection.headers()); i.has_next(); ){
			ResponseHeaders::Header header=i.next();
			if(!strcmp(name, header.name.cstr()))
				return header.value.cstrm();
		}
		return NULL;
	}

	static const char* mk_env_pair(const char* key, const char* value) {
		char *result=new(PointerFreeGC) char[5 /*HTTP_*/ + strlen(key) + 1 /*=*/ + strlen(value) + 1 /*0*/];
		strcpy(result, "HTTP_"); strcat(result, key); strcat(result, "="); strcat(result, value);
		return result;
	}

	virtual const char* const *get_env() {
		const char** result=new(PointerGC) const char*[connection.headers().count()+1/*0*/];
		const char** cur=result;
		for(Array_iterator<ResponseHeaders::Header> i(connection.headers()); i.has_next(); ){
			ResponseHeaders::Header header=i.next();
			*cur++=mk_env_pair(header.name.cstr(), header.value.cstr());
		}
		*cur=NULL;
		return result;
	}

	virtual size_t read_post(char *, size_t) {
		return 0;
	}

	virtual void add_header_attribute(const char* dont_store_key, const char* dont_store_value) {
		if(strcasecmp(dont_store_key, "location")==0)
			http_response_code=302;
		if(strcasecmp(dont_store_key, HTTP_STATUS)==0)
			http_response_code=atoi(dont_store_value);
		else
			output << capitalize(dont_store_key) << ": " << pa_strdup(dont_store_value) << "\r\n";
	}

	static const char *message(int code) {
		struct Lookup {
			int code;
			const char *message;
		} static lookup[] = {
			{200, "OK"},
			{206, "Partial Content"},
			{301, "Moved Permanently"},
			{302, "Found"},
			{400, "Bad Request"},
			{401, "Unauthorized"},
			{403, "Forbidden"},
			{404, "Not Found"},
			{500, "Internal Server Error"},
			{501, "Not Implemented"},
			{502, "Bad Gateway"},
			{504, "Gateway Timeout"},
			{  0, "Undescribed"}
		};
		Lookup *cur = lookup;
		for(; cur->code; cur++)
			if(code == cur->code)
				return cur->message;
		return cur->message;
	}

	virtual void send_header() {
		String result("HTTP/1.0 ");
		result << String::Body::Format(http_response_code) << " " << message(http_response_code) << "\r\n" << output << "\r\n";
		send_body(result.cstr(), result.length());
	}

	virtual size_t send_body(const void *buf, size_t size) {
		if(send(connection.sock, buf, size, 0)!=(ssize_t)size) {
			int no=pa_socks_errno();
			throw Exception("httpd.timeout", 0, "error sending response: %s (%d)", pa_socks_strerr(no), no);
		}
		return size;
	}

};

#endif
