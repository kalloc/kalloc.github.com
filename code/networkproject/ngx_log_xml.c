#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define LOG


static void * xml_log_create_conf(ngx_conf_t *cf);
static char * xml_log_merge_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t xml_log_init(ngx_conf_t *cf);
static ngx_int_t xml_log_handler(ngx_http_request_t *r);
static ngx_int_t xml_log_init_module(ngx_cycle_t *cycle);
static ngx_int_t xml_log_exit_process(ngx_cycle_t *cycle);

typedef struct {
    ngx_path_t *path;
    ngx_open_file_t *file;
    unsigned enable:1;
} xml_log_conf_t;

struct pos_st {
    unsigned key:1;
    unsigned value:1;
} pos;

char * buf;
unsigned fd;
unsigned len;


static ngx_command_t  xml_log_commands[] = {
  { ngx_string("xml_log_path"),
    NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_path_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(xml_log_conf_t, path),
    &ngx_garbage_collector_temp_handler,
  },
  ngx_null_command
};




static u_char reply[]="Hello World!!!\n";


static ngx_http_module_t  xml_log_module_ctx = {
    NULL,
    xml_log_init,
    NULL,
    NULL, 
    NULL,
    NULL, 
    xml_log_create_conf,
    xml_log_merge_conf, 
};


ngx_module_t  xml_log_module = {
    NGX_MODULE_V1,
    &xml_log_module_ctx, /* module context */
    xml_log_commands,   /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                  /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};




static void * xml_log_create_conf(ngx_conf_t *cf) {
    xml_log_conf_t *conf;
    conf = ngx_pcalloc(cf->pool,sizeof(xml_log_conf_t));
    if ( conf == NULL ) {
	return NGX_CONF_ERROR;
    }
    return conf;
}

static char * xml_log_merge_conf(ngx_conf_t *cf, void *parent, void *child) {
    xml_log_conf_t *prev = parent;
    xml_log_conf_t *conf = child;
    ngx_open_file_t *file;
    ngx_str_t *str;

    ngx_conf_merge_path_value(conf->path, prev->path, "/",1,2,0,ngx_garbage_collector_temp_handler,cf);

    if((&conf->path->name)->len > 1)  {
	conf->enable=1;
	str=ngx_pcalloc(cf->pool,sizeof(ngx_str_t));
	str->data=ngx_pcalloc(cf->pool,(&conf->path->name)->len+15);
	(&conf->path->name)->data[(&conf->path->name)->len]=0;
	ngx_snprintf(str->data,(&conf->path->name)->len+15,"%s/%d.xml.tmp",(&conf->path->name)->data,ngx_getpid());
	if((conf->file=ngx_conf_open_file(cf->cycle,str)) == NULL) return NGX_CONF_ERROR;
    }
    return NGX_CONF_OK;
}

#define cpy(a,b) ngx_cpymem(a,b,sizeof(b)-1)

static void * xml_log_write(ngx_http_request_t *r) {

    char *ptr,*wptr;
    wptr=buf;

    r->server_name.data[r->server_name.len]=0;
    r->uri.data[r->uri.len]=0;

    wptr=ngx_sprintf(wptr,"<log time=\"%d\" ip=\"%d\" host=\"%s\" path=\"%s\">\n",ngx_time(),((struct sockaddr_in *) r->connection->sockaddr)->sin_addr.s_addr,r->server_name.data,r->uri.data);


    unsigned off=0;

    if(r->method == NGX_HTTP_POST) {

	if(r->request_body && r->request_body->buf && (len=r->request_body->buf->last - r->request_body->buf->start) && len > 0) {

	    pos.key=0;
	    pos.value=0;
	    ptr=r->request_body->buf->start;
            for(;len > 0;ptr++,len--) {
			if(pos.key == 0 && pos.value == 0) {
			    pos.key=1;
			    wptr=cpy(wptr,"\t<query method=\"POST\" name=\"");
			}
			
			if(pos.key ==1 && pos.value == 0) {
			    if(*ptr == '=') {
				wptr=cpy(wptr,"\" empty=\"0\"><![CDATA[");
				pos.key=0;
				pos.value=1;
			    } else if(*ptr == '&') {
				wptr=cpy(wptr,"\" empty=\"1\"/>\n");
				pos.key=pos.value=0;
			    } else {
				wptr[0]=*ptr;
				wptr++;
			    }
			} else if(pos.key ==0 && pos.value == 1) {
			    if(*ptr == '&') {
				wptr=cpy(wptr,"]]></query>\n");
				pos.key=pos.value=0;
			    } else {
				wptr[0]=*ptr;
				wptr++;
			    }
			}
            }
    
        if(pos.key == 1)  wptr=cpy(wptr,"\" empty=\"1\" />\n");
        else wptr=cpy(wptr,"]]></query>\n");

	}
    }

    if(r->args.len > 0) {
        pos.key=0;
	pos.value=0;
	ptr=r->args.data;
	len=r->args.len;
        for(;len > 0;ptr++,len--) {
		if(pos.key == 0 && pos.value == 0) {
		    pos.key=1;
		    wptr=cpy(wptr,"\t<query method=\"GET\" name=\"");
		}
		
		if(pos.key ==1 && pos.value == 0) {
		    if(*ptr == '=') {
			wptr=cpy(wptr,"\" empty=\"0\"><![CDATA[");
			pos.key=0;
			pos.value=1;
		    } else if(*ptr == '&') {
			wptr=cpy(wptr,"\" empty=\"1\"/>\n");
			pos.key=pos.value=0;
		    } else {
			wptr[0]=*ptr;
			wptr++;
		    }
		} else if(pos.key ==0 && pos.value == 1) {
		    if(*ptr == '&') {
			wptr=cpy(wptr,"]]></query>\n");
			pos.key=pos.value=0;
		    } else {
			wptr[0]=*ptr;
			wptr++;
		    }
		}
        }
        if(pos.key == 1)  wptr=cpy(wptr,"\" empty=\"1\" />\n");
        else wptr=cpy(wptr,"]]></query>\n");

    }

    wptr=cpy(wptr,"</log>\n");
    wptr[0]=0;
    len=(unsigned)(wptr-buf);
    ngx_write_fd(fd,buf,wptr-buf);

}



static ngx_int_t xml_log(ngx_http_request_t *r) {

    xml_log_conf_t *cf;
    ngx_int_t     rc;


    if (!r->method) return NGX_DECLINED;

    cf  = ngx_http_get_module_loc_conf(r, xml_log_module);

    if(!cf->enable) return NGX_DECLINED;

    rc = ngx_http_read_client_request_body(r, xml_log_handler);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE || rc == NGX_DECLINED || rc ==NGX_AGAIN) {
            return rc;
    } 



#ifdef LOG
    return NGX_DECLINED;
#else
    ngx_http_finalize_request(r, ngx_http_send_header(r));
    return NGX_DONE;
#endif
}

static ngx_int_t xml_log_handler(ngx_http_request_t *r) {

    ngx_buf_t    *b;
    ngx_chain_t   out;
    ngx_int_t     rc;
    xml_log_conf_t *cf;


    cf  = ngx_http_get_module_loc_conf(r, xml_log_module);

    fd=cf->file->fd;

    buf=ngx_pcalloc(r->pool,r->request_length*2);

    xml_log_write(r);


#ifndef LOG
    r->headers_out.content_type.len = sizeof("text/xml") - 1;
    r->headers_out.content_type.data = (u_char *) "text/xml";


    if (r->method == NGX_HTTP_HEAD) {
        r->headers_out.status = NGX_HTTP_OK;
//        r->headers_out.content_length_n = sizeof(reply);
        r->headers_out.content_length_n = len;
        r->headers_out.last_modified_time = 23349600;
        return ngx_http_send_header(r);
    }

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    if (b == NULL) {

            return NGX_HTTP_INTERNAL_SERVER_ERROR;

    }

    out.buf = b;
    out.next = NULL;

//    b->pos = reply;
//    b->last = reply + sizeof(reply);
    b->pos = buf;
    b->last = buf + len;
    b->memory = 1;
    b->last_buf = 1;
    b->last_in_chain = 1;
    r->headers_out.status = NGX_HTTP_OK;
//    r->headers_out.content_length_n = sizeof(reply);
    r->headers_out.content_length_n = len;
    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }
    return ngx_http_output_filter(r, &out);


#endif



}

static ngx_int_t xml_log_init(ngx_conf_t *cf) {



    ngx_http_core_main_conf_t	*conf_m;
    ngx_http_handler_pt		*h;
    conf_m =  ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

#ifdef LOG
    if ((h = ngx_array_push(&conf_m->phases[NGX_HTTP_LOG_PHASE].handlers)) == NULL) return NGX_ERROR;
    *h = xml_log;
#else
    if ((h = ngx_array_push(&conf_m->phases[NGX_HTTP_SERVER_REWRITE_PHASE].handlers)) == NULL) return NGX_ERROR;
    *h = xml_log;
#endif


    return NGX_OK;
}

