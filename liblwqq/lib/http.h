/**
 * @file   http.h
 * @author mathslinux <riegamaths@gmail.com>
 * @date   Mon May 21 23:07:29 2012
 *
 * @brief  Linux WebQQ Http API
 *
 *
 */

#ifndef LWQQ_HTTP_H
#define LWQQ_HTTP_H

#include <stdio.h>
#include "type.h"
#include "async.h"

typedef enum {
    LWQQ_FORM_FILE,// use add_file_content instead
    LWQQ_FORM_CONTENT
} LWQQ_FORM;
typedef enum {
    LWQQ_HTTP_TIMEOUT,
    LWQQ_HTTP_ALL_TIMEOUT,
    LWQQ_HTTP_NOT_FOLLOW,
    LWQQ_HTTP_SAVE_FILE,
    LWQQ_HTTP_RESET_URL,
    LWQQ_HTTP_VERBOSE,
    LWQQ_HTTP_CANCELABLE,
    LWQQ_HTTP_MAXREDIRS,
    LWQQ_HTTP_NOT_SET_COOKIE = 1<<7,
	LWQQ_HTTP_MAX_LINK = 1000
}LwqqHttpOption;
/**
 * Lwqq Http request struct, this http object worked done for lwqq,
 * But for other app, it may work bad.
 *
 */
struct _LwqqHttpRequest {
    void *req;
    void *lc;
    void *header;// read and write.
    void *recv_head;
    void *form_start;
    void *form_end;

    /**
     * Http code return from server. e.g. 200, 404, this maybe changed
     * after do_request() called.
     */
    long http_code;
    LwqqCallbackCode failcode;
    int retry;

    /* Server response, used when do async request */
    char *location;
    char *response;

    /* Response length, NB: the response may not terminate with '\0' */
    int resp_len;

    /**
     * Send a request to server, method is GET(0) or POST(1), if we make a
     * POST request, we must provide a http body.
     */
    int (*do_request)(LwqqHttpRequest *request, int method, char *body);

    /**
     * Send a request to server asynchronous, method is GET(0) or POST(1),
     * if we make a POST request, we must provide a http body.
     */
    LwqqAsyncEvent* (*do_request_async)(LwqqHttpRequest *request, int method,
                            char *body,LwqqCommand);

    /* Set our http client header */
    void (*set_header)(LwqqHttpRequest *request, const char *name,
                       const char *value);

    /** Get header */
    const char * (*get_header)(LwqqHttpRequest *request, const char *name);

    //add http form
    void (*add_form)(LwqqHttpRequest* request,LWQQ_FORM form,const char* name,const char* content);
    //add http form file type
    void (*add_file_content)(LwqqHttpRequest* request,const char* name,const char* filename,const void* data,size_t size,const char* extension);
    //progressing function callback
    int (*progress_func)(void* data,size_t now,size_t total);
    void* prog_data;
    time_t last_prog;
} ;

typedef struct LwqqHttpHandle{
    struct {
        enum {
            LWQQ_HTTP_PROXY_NOT_SET = -1, //let curl auto set proxy
            LWQQ_HTTP_PROXY_NONE,
            LWQQ_HTTP_PROXY_HTTP,
            LWQQ_HTTP_PROXY_SOCKS4,
            LWQQ_HTTP_PROXY_SOCKS5
        }type;
        char* host;
        int port;
        char* username;
        char* password;
    }proxy;
    int quit;
    int synced;
	int ssl;
}LwqqHttpHandle;

LwqqHttpHandle* lwqq_http_handle_new();
void lwqq_http_handle_free(LwqqHttpHandle* http);
#define lwqq_http_proxy_set(_handle,_type,_host,_port,_username,_password)\
do{\
    LwqqHttpHandle* h = (LwqqHttpHandle*) (_handle);\
    h->proxy.type = _type;\
    h->proxy.host = s_strdup(_host);\
    h->proxy.port = _port;\
    h->proxy.username = s_strdup(_username);\
    h->proxy.password = s_strdup(_password);\
}while(0);

#define lwqq_http_ssl(lc) (lwqq_get_http_handle(lc)->ssl)
#define __SSL lwqq_http_ssl(lc)
#define __H(url) __SSL?"https://"url:"http://"url
#define WEBQQ_D_REF_URL (__SSL)?\
							"https://d.web2.qq.com/cfproxy.html?v=20110331002&callback=1":\
							"http://d.web2.qq.com/proxy.html?v=20110331002&callback=1"
#define WEBQQ_D_HOST        __H("d.web2.qq.com")

void lwqq_http_proxy_apply(LwqqHttpHandle* handle,LwqqHttpRequest* req);

/**
 * Free Http Request
 * always return 0
 *
 * @param request
 */
int lwqq_http_request_free(LwqqHttpRequest *request);

/**
 * Create a new Http request instance
 *
 * @param uri Request service from
 *
 * @return
 */
LwqqHttpRequest *lwqq_http_request_new(const char *uri);

/**
 * Create a default http request object using default http header.
 *
 * @param url Which your want send this request to
 * @param err This parameter can be null, if so, we dont give thing
 *        error information.
 *
 * @return Null if failed, else a new http request object
 */
LwqqHttpRequest *lwqq_http_create_default_request(LwqqClient* lc,const char *url,
        LwqqErrorCode *err);

void lwqq_http_global_init();
void lwqq_http_global_free();
/** stop a client all http progressing request */
void lwqq_http_cleanup(LwqqClient*lc);
/** set the other option of request, like curl_easy_setopt */
void lwqq_http_set_option(LwqqHttpRequest* req,LwqqHttpOption opt,...);
/** regist http progressing callback */
void lwqq_http_on_progress(LwqqHttpRequest* req,LWQQ_PROGRESS progress,void* prog_data);
const char* lwqq_http_get_url(LwqqHttpRequest* req);

char *lwqq_http_get_cookie(LwqqHttpHandle* h, const char *name);
void  lwqq_http_set_cookie(LwqqHttpRequest *request, const char *name,const char* val);
/** 
 * force stop a request 
 * require set LWQQ_HTTP_CANCELABLE option first
 * invoke callback with failcode = LWQQ_EC_CANCELED
 */
void lwqq_http_cancel(LwqqHttpRequest* req);

#endif  /* LWQQ_HTTP_H */
