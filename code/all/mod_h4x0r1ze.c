#include "ap_config.h"

#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_log.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

#include "util_filter.h"

#include "apr_buckets.h"
#include "apr_pools.h"
#include "apr_strings.h"
#include "apr_general.h"
#include "apr_hash.h"
#include "apr_lib.h"



module AP_MODULE_DECLARE_DATA h4x0r1ze_module ;

static const char* h4x0r1ze_name = "h4x0r1ze-filter" ;


static apr_bucket* h4x0r1ze_esc(char c, apr_bucket_alloc_t* alloc ) {
  switch (c) {
    case 'a': return apr_bucket_transient_create("4", 1, alloc) ;
    case 'e': return apr_bucket_transient_create("3", 1, alloc) ;
    case 'i': return apr_bucket_transient_create("1", 1, alloc) ;
    case 'b': return apr_bucket_transient_create("8", 1, alloc) ;
    case 'l': return apr_bucket_transient_create("1", 1, alloc) ;
    case 't': return apr_bucket_transient_create("7", 1, alloc) ;
    case 'o': return apr_bucket_transient_create("0", 1, alloc) ;
    default: return NULL ;
  }
}

static int post_config_hook(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{


        ap_add_version_component(p," h4x0rize/3.1337");
	return APR_SUCCESS;


}



static int h4x0r1ze_filter(ap_filter_t* f, apr_bucket_brigade* bb) {
  apr_bucket* b ;
  request_rec *r = f->r;


  char *ce;
    if (r->header_only)
	return ap_pass_brigade(f->next, bb) ;


  ce=apr_table_get(r->headers_out,"Content-Encoding");


  if(r->args && strstr(r->args,"h4x0r") != NULL && (ce == NULL  || strcmp(ce,"gzip")))
  {

  ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server, "mod_h4x0rize: %s",r->filename);

  for ( b = APR_BRIGADE_FIRST(bb);b != APR_BRIGADE_SENTINEL(bb);b = APR_BUCKET_NEXT(b) ) {

    const char* buf,* pOpen,* pClose ;
    size_t bytes ;
    size_t bytesCheckOpen,byteOpen,byteClose ;
     if ( apr_bucket_read(b, &buf, &bytes, APR_BLOCK_READ) == APR_SUCCESS ) {
      /* We have a bucket full of text.  Just escape it where necessary */
      size_t count = 0 ;
      size_t sz ;
      const char* p = buf ;
      while ( count < bytes ) {

	pOpen=strstr(p,"<");
	pClose=strstr(p,">");
	byteOpen=pOpen != NULL ? (pOpen-p+1) : 0;
	byteClose=pClose != NULL ? (pClose-p+1) : 0;
	sz = strcspn(p, "aeiolt") ;



      //ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server, "mod_h4x0rize: %s",p);
      //ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, r->server, "mod_h4x0rize: pOpen=%i pClose=%i sz=%i byteClose=%i byteOpen=%i ",pOpen,pClose,sz,byteClose,byteOpen);

      if(( (pOpen  && pClose &&  sz < byteClose)  && !(byteOpen > sz && byteClose > sz)) || p[sz-1] == '<'  || p[sz+1] == '>' )
      {

	  count += byteClose;
	  p = pClose+1;
//  	  count += sz ;
	  count += 1 ;
//	  p += sz + 1 ;
	  apr_bucket_split(b, byteClose) ;
	  b = APR_BUCKET_NEXT(b) ;
	  
      }
      else 
      {
	
	count += sz ;


	if ( count < bytes ) {

	  apr_bucket_split(b, sz) ;
	  b = APR_BUCKET_NEXT(b) ;
	  APR_BUCKET_INSERT_BEFORE(b, h4x0r1ze_esc(p[sz],f->r->connection->bucket_alloc)) ;
	  apr_bucket_split(b, 1) ;
	  APR_BUCKET_REMOVE(b) ;
	  b = APR_BUCKET_NEXT(b) ;
	  count += 1 ;
	  p += sz + 1 ;
	}
       }

      }
    }
  }

 }
  return ap_pass_brigade(f->next, bb) ;
}

static void h4x0r1ze_hooks(apr_pool_t* p) {
  ap_hook_post_config(post_config_hook, NULL, NULL, APR_HOOK_LAST);
  ap_register_output_filter(h4x0r1ze_name, h4x0r1ze_filter, NULL,AP_FTYPE_RESOURCE) ;
}

module AP_MODULE_DECLARE_DATA h4x0r1ze_module = {
	STANDARD20_MODULE_STUFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	h4x0r1ze_hooks
};

