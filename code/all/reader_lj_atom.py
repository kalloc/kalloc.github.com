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
import asyncore
import string
import re
import codecs
import csv


ljuser={}
ljpool=[]
ljquery=[]


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
	    c.setopt(URL,str(url))
	    prx_num=random.randint(0,len(proxy_list)-1)
	    c.setopt(PROXY,proxy_list[prx_num])
	    c.setopt(WRITEFUNCTION,c.res.write)
#	    print "use PROXY",proxy_list[prx_num]
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
#                print "FETCH OK:", c.getinfo(EFFECTIVE_URL),
                worker.append(c)
		for mask in func:
		    if re.findall(mask[1],c.url):
#			print " open by mask",mask[1],c.url
			mask[0](c)
            for c, errno, errmsg in err_list:
                m.remove_handle(c)
		c.data=c.res.getvalue()
		pool.append(url)
#                print "FETCH FAIL:",c.getinfo(EFFECTIVE_URL),errno,errmsg
                worker.append(c)
            num_processed = num_processed + len(ok_list)
            if num_q == 0:
                break
        m.select(1.0)
    return




class lj_parser:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        #print "GET",self.host + self.path ,client.status[0] , client.status[1]

        if (client.status[1] == "301" or client.status[1] == "302"):
	    http_client.do_request(header['location'], lj_parser())

    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self): 
	nick=re.findall("""<foaf:nick>([^<]+)</foaf:nick>""",self.data)
	name=re.findall("""<foaf:name>([^<]+)</foaf:name>""",self.data)
	ljnick=re.findall("""<foaf:Document rdf:about="http://([^.]+)""",self.data)
	img=re.findall("""<foaf:img rdf:resource="([^"]+)" />""",self.data)
	icq=re.findall("""<foaf:icqChatID>([^<]+)</foaf:icqChatID>""",self.data)
	date=re.findall("""<foaf:dateOfBirth>([^<]+)</foaf:dateOfBirth>""",self.data)
	posted=re.findall("""<ya:posted>([^<]+)</ya:posted>""",self.data)

	if nick and not nick[0] in ljuser:
	    ljuser[nick[0]]=1
	    ljpool.append([nick and nick[0] or "",ljnick and ljnick[0] or "",name and name[0] or "",img and img[0] or "",date and date[0] or "",posted and posted[0] or "",icq and icq[0] or ""])
	    #print self.host + self.path + ", read:",len(self.data)
	    if len(ljpool) > 50:
		write()


	if  len(ljquery) > 50:  getit()

	return 


def lj_parser_recurse(self):
	nick=re.findall("""<foaf:nick>([^<]+)</foaf:nick>""",self.data)
	name=re.findall("""<foaf:name>([^<]+)</foaf:name>""",self.data)
	ljnick=re.findall("""<foaf:Document rdf:about="http://([^.]+)""",self.data)
	img=re.findall("""<foaf:img rdf:resource="([^"]+)" />""",self.data)
	date=re.findall("""<foaf:dateOfBirth>([^<]+)</foaf:dateOfBirth>""",self.data)
	icq=re.findall("""<foaf:icqChatID>([^<]+)</foaf:icqChatID>""",self.data)
	posted=re.findall("""<ya:posted>([^<]+)</ya:posted>""",self.data)
	seealso=re.findall("""<rdfs:seeAlso rdf:resource="http://([^\.]+).livejournal.com/data/foaf" />""",self.data)
	counter=0
	counter_exists=0
	if nick:
	    for login in seealso:
	        if login and login != "community" and  (ljuser.get(login,None) ==  None) and not "http://"+login+".livejournal.com/data/foaf" in ljquery:
		    ljuser[login]=1
		    ljquery.append("http://"+login+".livejournal.com/data/foaf")
		    counter+=1
		else:
    		    counter_exists+=1
		    

	    ljuser[ljnick[0]]=1
	    ljpool.append([nick and nick[0] or "",ljnick and ljnick[0] or "",name and name[0] or "",img and img[0] or "",date and date[0] or "",posted and posted[0] or "",icq and icq[0] or ""])
	    print self.url+ ", read:",len(self.data),", add: ",counter,"already exists: ",counter_exists
        if len(ljpool) > 50:
	    write()
	
	return 


filename = "ljlogin.txt"
reader = csv.reader(open(filename, "r"),dialect=csv.excel)
for row in reader:
    ljuser[row[1]]=1
#    print row[1]    
    
writer = csv.writer(open(filename, "ab"),dialect=csv.excel)

def write():
    writer.writerows(ljpool)
    del ljpool[0:len(ljpool)]

def getit():
    while len(ljquery) >50:
        url=[]
        while len(ljquery)>0:
    	    zzz=ljquery.pop()
    	    if not zzz in url: url.append(zzz)
	del ljquery[:]
        fetch(url,[[lj_parser_recurse,"data/foaf"]],50)



import atomstream
import feedparser
from itertools import imap



while 1:
    try:
	for update in imap(feedparser.parse, atomstream.connect()):
    	    login=re.findall("http://(.+)\.livejournal\.com",update.feed.link)
    	    if login and login[0] != "community" and  (ljuser.get(login[0],None) ==  None):
    		ljuser[login[0]]=1
    		print "fetch from atom",login[0]
        	ljquery.append("http://"+login[0]+".livejournal.com/data/foaf")
		getit()
    except:
	pass

write()



def getit():
    while len(query) >0:
	url=[]
	while len(query)>0:
	    url.append(query.pop())
	del query[:]
	fetch(url,[[parser,""]],20)




