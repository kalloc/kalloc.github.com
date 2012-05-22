#!/usr/bin/python
# -*- coding: utf8 -*-

import http_client, asyncore
import string
import re
import codecs
import csv


def info(v):
     return '%s = %r %s' % (v, v, type(v))

ljuser = []


class parser:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        if (client.status[1] == "301"):
	    http_client.do_request(header['location'], parser())
	else:
    	    print self.host + self.path +" "+ client.status[0] + " " + client.status[1]

    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self): 
#	print re.findall("""id="next_page" href="/top/lj/?page=(\d+)""",self.data);
	#next=re.findall("""id="next_page" href="/top/lj/\?page=(\d+)""",self.data);
	self.data=unicode(self.data,"cp1251").encode("utf-8")
	logins=re.findall("""<a href="http://([^/]+)/" title="[^"]+">[^<]+</a>""",self.data)
	for login in logins:
	    http_client.do_request("http://"+login+"/data/foaf", lj_parser())
#	http_client.do_request("http://"+logins[0]+"/data/foaf", lj_parser())
	next=re.findall("""id="next_page" href="/top/lj/\?page=(\d+)""",self.data);
	if(next):
	    http_client.do_request(URLS[0]+"?page="+next[0], parser())
	return 

class lj_parser:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        if (client.status[1] == "301"):
	    http_client.do_request(header['location'], parser())
	else:
    		print self.host + self.path +" "+ client.status[0] + " " + client.status[1]

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
	date=re.findall("""<foaf:dateOfBirth>([^<]+)</foaf:dateOfBirth>""",self.data)
	posted=re.findall("""<ya:posted>([^<]+)</ya:posted>""",self.data)

	if nick:
	    ljuser.append((nick and nick[0] or "",ljnick and ljnick[0] or "",name and name[0] or "",img and img[0] or "",date and date[0] or "",posted and posted[0] or ""))

	if(len(ljuser) >= 50):
	    writer.writerows(ljuser)
	    for i in range(0,len(ljuser)):
		ljuser.pop()


	return 



URLS = ("http://blog.yandex.ru/top/lj/",)

filename = "ljlogin.txt"
writer = csv.writer(open(filename, "wb"),dialect=csv.excel)


for url in URLS:
    http_client.do_request(url, parser())

asyncore.loop()

writer.writerows(ljuser)


