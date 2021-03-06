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


offset=0
user=[]
query=[]



def isset(where,who):
    try:
	return getattr(where,who)
    except:
    	return 0

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
	c.setopt(HTTPHEADER, ['User-Agent: Mozilla/5.0 (rv:1.8.1.12) Gecko/20080129 Firefox/2.0.0.14 Windows; U; Windows NT 6.0; en-US; rv:1.8.1.14'])
	c.setopt(NOSIGNAL, 1)
	m.handles.append(c)

    worker = m.handles[:]
        
    while num_pool > num_processed:
	while worker and pool:
	    url = pool.pop()
	    c= worker.pop()
	    c.res = cStringIO.StringIO()
	    c.setopt(URL,url)
	    prx_num=random.randint(0,len(proxy_list)-1)
	    c.setopt(PROXY,proxy_list[prx_num])
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
                print "FETCH OK:", c.getinfo(EFFECTIVE_URL),
                worker.append(c)
		for mask in func:
		    if re.findall(mask[1],c.url):
			print " open by mask",mask[1],c.url
			mask[0](c)
            for c, errno, errmsg in err_list:
                m.remove_handle(c)
		c.data=c.res.getvalue()
		pool.append(url)
                print "FETCH FAIL:",c.getinfo(EFFECTIVE_URL),errno,errmsg
                worker.append(c)
            num_processed = num_processed + len(ok_list)
            if num_q == 0:
                break
        m.select(1.0)
    return


def main_parser(self): 
    logins=re.findall("""<a href=/Profile.jsp\?MemberId=([0-9]+)>""",self.data)
    print self.data
    print logins
    sys.exit(0)
    for login in logins:
	user.append([login])
	writer.writerow([login])
    next=re.findall("""<a href='(\?SearchType=[^']+)'>Next &gt;&gt;</a>""",self.data)
    if(next):
	query.append("http://www.bebo.com/Search2.jsp"+next[0])
    getit()
    return 



filename = "bebologin.txt"
try:
    fp = open(filename,'r')
except:
    fp = 0

if  fp != 0:
    fp.close()
    reader = csv.reader(open(filename, "r"),dialect=csv.excel)
    for row in reader:
        user.append(row[0])


writer = csv.writer(open(filename, "ab"),dialect=csv.excel)






def getit():
    while len(query) >0:
	url=[]
	while len(query)>0:
	    url.append(query.pop())
	del query[:]
	fetch(url,[[main_parser,""]],20)

URLS = [
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=home",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=me",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=my",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=you",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=for",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=im",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=right",
]


query=URLS

getit()








writer.writerows(user)


