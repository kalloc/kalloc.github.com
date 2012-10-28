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
count_error={}
user=[]
query=[]
URLS = []


proxy_list=[]
f = file("proxy.txt", 'r')
for line in f.read().split("\n"):
    if re.match("""[0-9\.]+:[0-9]+""",line  ):
        proxy=line.strip()
        proxy_list.append(proxy)
f.close()
if  len(proxy_list) == 0:
    print "unable to loading proxy from list"
    sys.exit()
    
prx_num=random.randint(0,len(proxy_list)-1)
proxy=proxy_list[prx_num]




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
	c.setopt(HTTPHEADER, ['User-Agent: Mozilla/5.0 (rv:1.8.1.12) Gecko/20080129 Firefox/2.0.0.14 Windows; U; Windows NT 6.0; en-US; rv:1.8.1.14','Cookie: Gen=M; Email=yourbunnywrite@yandex.ru; Age=25; Username=JohnD6723; bdaysession=2349da604903461840583943; sessioncreate=20080527124759'])
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
	    prx_num=random.randint(0,len(proxy_list)-1)
	    c.setopt(PROXY,proxy_list[prx_num])
	    c.setopt(WRITEFUNCTION,c.res.write)
#	    print "use PROXY",proxy_list[prx_num]
	    m.add_handle(c)
	    c.url=url
	    c.proxy=proxy_list[prx_num]
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
#			print " open by mask",mask[1],c.url
	#		try:
			mask[0](c)
	#		except:
	#		    pass
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


def parser_user(self): 
    self.data=unicode(self.data,"windows-1251")
    self.data=self.data.encode("utf-8")
    names=re.findall("""id=[0-9]+&gender=([0-9]+)">([^<]+)<\/a>""",self.data)
    fp.writerows(names)
    return 


def parser_main(self):
    names=re.findall("""(\?name=[^&]+&gender=)""",self.data);
    if not names: query.append(self.url)
    for name in names:
	query.append("http://names.astrostar.ru/"+name+"1&p=0")
	query.append("http://names.astrostar.ru/"+name+"1&p=1")
	query.append("http://names.astrostar.ru/"+name+"1&p=2")
	query.append("http://names.astrostar.ru/"+name+"1&p=3")
	query.append("http://names.astrostar.ru/"+name+"0&p=1")
	query.append("http://names.astrostar.ru/"+name+"0&p=2")
	query.append("http://names.astrostar.ru/"+name+"0&p=3")
	query.append("http://names.astrostar.ru/"+name+"0&p=0")




fp= csv.writer(open("names.txt", "ab"),dialect=csv.excel)




fetch(["http://names.astrostar.ru/"],[[parser_main,""]],len(proxy_list)/2)

def getit():
    while len(query) >0:
	url=[]
	while len(query)>0:
	    url.append(query.pop())
	del query[:]
	fetch(url,[[parser_user,"\?name="]],len(proxy_list)/2)
getit()











