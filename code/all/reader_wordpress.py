#!/usr/bin/python
# -*- coding: utf8 -*-

import sys 
from pycurl import *
import cStringIO
import urllib,time,random
import string
import re
import os
import codecs
import csv
from lxml.html import fromstring,tostring


offset=0


def fetch(pool,func,conn = 10):
    num_processed = 0
    num_pool = len(pool)
    ret=[]
    conn = min(num_pool,conn)
    m = CurlMulti()
    m.handles = []
    for i in range(conn):
	c = Curl()
	c.setopt(FOLLOWLOCATION, 1)
        c.setopt(MAXREDIRS, 5)
        c.setopt(CONNECTTIMEOUT, 1)
	c.setopt(TIMEOUT, 30)
	c.setopt(NOSIGNAL, 1)
	m.handles.append(c)

    worker = m.handles[:]
        
    while num_pool > num_processed:
	while worker and pool:
	    url = pool.pop()
	    while url in pool:
		pool.remove(url)
		num_pool=num_pool-1

	    c= worker.pop()
	    c.res = cStringIO.StringIO()
	    c.setopt(URL,url)
	    if url.find("?") > 0:
		id=url[url.rindex("=")+1:]	    
	        c.setopt(HTTPHEADER, ['User-Agent: Mozilla/5.0 (rv:1.8.1.12) Gecko/20080129 Firefox/2.0.0.14 Windows; U; Windows NT 6.0; en-US; rv:1.8.1.14','Cookie: wpthemefd29dcdc626e8c0636e3096c1528b727='+id])
	    else:
	        c.setopt(HTTPHEADER, ['User-Agent: Mozilla/5.0 (rv:1.8.1.12) Gecko/20080129 Firefox/2.0.0.14 Windows; U; Windows NT 6.0; en-US; rv:1.8.1.14'])
	    c.setopt(WRITEFUNCTION,c.res.write)
	    m.add_handle(c)
	    c.url=url
	while 1:
	    ret,num_h = m.perform()
	    if ret != E_CALL_MULTI_PERFORM: break
	    
        while 1:
            num_q, ok_list, err_list = m.info_read()
            for c in ok_list:
                m.remove_handle(c)
		c.data=c.res.getvalue()
                print "FETCH OK:", c.getinfo(EFFECTIVE_URL)
                worker.append(c)
		for mask in func:
		    if re.findall(mask[1],c.url):
			print " open by mask",mask[1],c.url
			try:
			    mask[0](c)
			except:
			    print sys.exc_info()
			    #sys.exit(0)
			    pass
            for c, errno, errmsg in err_list:
                m.remove_handle(c)
		c.data=c.res.getvalue()
		pool.append(url)
                print "FETCH FAIL:",c.getinfo(EFFECTIVE_URL),errno,errmsg
                worker.append(c)
            num_processed = num_processed + len(ok_list)
            if num_q == 0:
                break
	try:
	    m.select(1.0)
	except:
	    pass
    return


def parser(self): 
    id=re.findall("""http://themes.wordpress.net/testrun/wp-content/themes/([0-9]+)/""",self.data)
    if not id: return
    id=id[0]

    urls1=re.findall("""(http://themes.wordpress.net/testrun/wp-content/themes/[^"'\s\t\)]+|http://s.themes.wordpress.net/images/[^"'\t\s\)]+|http://themes.wordpress.net/testrun/[^/]+/[^/]+/[^"'\t\s]+|http://themes.wordpress.net/testrun/[^/]+/[^"'\t\s]+)""",self.data)
    urls2=re.findall("""url\s?\(['"]?(.*?)['"]?\)""",self.data)
    for url in urls1:
	if url.find("?") < 0 and url.find("\"")  <0:
	    url=url.strip()
	    query.append(url+"?id="+id)
	    self.data=self.data.replace(url,"images/"+url[url.rindex("/")+1:])


    for url in urls2:
        url=url.strip()
        if len(url)> 7 and url[:7] == "http://":
	    if url.find("?")>0:  query.append(url+"&id="+id)
	    else:   query.append(url+"?id="+id)
	else:
	    if url.find("?")>0:  query.append(self.url[0:self.url.rindex("/")]+"/"+url+"&id="+id)
	    else:   query.append(self.url[0:self.url.rindex("/")]+"/"+url+"?id="+id)



    if not os.path.exists("themes/"+id):
	os.mkdir("themes/"+id)

    self.data=re.sub('(<[Ss][Cc][Rr][Ii][Pp][Tt])','{literal}<script',self.data)
    self.data=re.sub('(</[Ss][Cc][Rr][Ii][Pp][Tt]>)','</script>{/literal}',self.data)
    self.data=re.sub('(<[Ss][Tt][Yy][Ll][Ee])','{literal}<style',self.data)
    self.data=re.sub('(</[Ss][Tt][Yy][Ll][Ee]>)','</style>{/literal}',self.data)
    fp=open("themes/"+id+"/index.tpl","wb")
    fp.write(self.data)
    fp.close()
    return 

def parser_static(self): 
    id=re.findall("""\?id=([0-9]+)""",self.url)[0]
    file=self.url[self.url.rindex("/")+1:].replace("?id="+id,"").replace("&id="+id,"")
    if file == "": return

    "если это css то парсим"
    if re.search(""".css$""",file,re.I):
        urls1=re.findall("""(http://themes.wordpress.net/testrun/wp-content/themes/[^'"\s\t\)]+|http://s.themes.wordpress.net/images/[^'"\t\s\)]+)""",self.data)
	urls2=re.findall("""url\s?\(['"]?(.*?)['"]?\)""",self.data)

	for url in urls1:
	    if url.find("?") < 0:
		url=url.strip()
		query.append(url+"?id="+id)
		self.data=self.data.replace(url,"images/"+url[url.find("/") > 0 and url.find("/")+1 or 0:])

	for url in urls2:
	    url=url.strip()
	    if len(url)> 7 and url[:7] == "http://":
		if url.find("?")>0:  query.append(url+"&id="+id)
	        else:   query.append(url+"?id="+id)
	    else:
		if url.find("?")>0:  query.append(self.url[0:self.url.rindex("/")]+"/"+url+"&id="+id)
	        else:   query.append(self.url[0:self.url.rindex("/")]+"/"+url+"?id="+id)
	    self.data=self.data.replace(url,"images/"+url[url.find("/") > 0 and url.find("/")+1 or 0:])
	


    if not os.path.exists("themes/"+id+"/images/"):
	os.mkdir("themes/"+id+"/images/")
    fp=open("themes/"+id+"/images/"+urllib.unquote(file),"wb")
    fp.write(self.data)
    fp.close()
    return 






def getit():
    while len(query) >0:
	url=[]
	while len(query)>0:
	    url.append(query.pop())
	del query[:]
	fetch(url,[[parser_static,"(themes/|images/)"],[parser,"testrun/\?wptheme=[0-9]+"]],20)




offset=1


while offset<7000:
    query=[]
    for offset in range(offset,offset+100,1):
	query.append("http://themes.wordpress.net/testrun/?wptheme="+str(offset))
    offset=offset+100
    getit()







