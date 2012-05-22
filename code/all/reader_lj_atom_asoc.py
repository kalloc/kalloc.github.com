#!/usr/bin/python
# -*- coding: utf8 -*-

import http_client, asyncore
import string
import re
import codecs
import csv


ljuser={}
ljpool=[]
ljquery=[]
ljpoolsize=[]


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

	if nick:
	    ljuser[nick[0]]=1
	    ljpool.append([nick and nick[0] or "",ljnick and ljnick[0] or "",name and name[0] or "",img and img[0] or "",date and date[0] or "",posted and posted[0] or "",icq and icq[0] or ""])
	    #print self.host + self.path + ", read:",len(self.data)
	    if len(ljpool) > 50:
		write()

	    if len(ljpoolsize) > 0:
		ljpoolsize.pop()

	if len(ljpoolsize) < 200 and len(ljquery) > 0:
	    getit()

	return 


class lj_parser_recurse:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        print "RECURSE GET",self.host + self.path ,client.status[0] , client.status[1]
        if (client.status[1] == "301" or client.status[1] == "302"):
	    http_client.do_request(header['location'], lj_parser_recurse())

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
	icq=re.findall("""<foaf:icqChatID>([^<]+)</foaf:icqChatID>""",self.data)
	posted=re.findall("""<ya:posted>([^<]+)</ya:posted>""",self.data)
	seealso=re.findall("""<rdfs:seeAlso rdf:resource="http://([^\.]+).livejournal.com/data/foaf" />""",self.data)
	counter=0
	counter_exists=0
	if nick:
	    for login in seealso:
	        if login and login != "community" and  (ljuser.get(login,None) ==  None):
		    ljuser[login]=1
		    ljquery.append("http://"+login+".livejournal.com/data/foaf")
		    counter+=1
		else:
    		    counter_exists+=1
		    

	    ljuser[ljnick[0]]=1
	    ljpool.append([nick and nick[0] or "",ljnick and ljnick[0] or "",name and name[0] or "",img and img[0] or "",date and date[0] or "",posted and posted[0] or "",icq and icq[0] or ""])
	    print self.host + self.path + ", read:",len(self.data),", add: ",counter,"already exists: ",counter_exists
	    if len(ljpoolsize) > 0:
		ljpoolsize.pop()

        if len(ljpool) > 50:
	    write()
	if len(ljpoolsize) < 30 and len(ljquery) > 0:
	    getit()
	
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
    while len(ljpoolsize)<200 and len(ljquery) >0:
	url=ljquery.pop()
	#print "do request",url
	http_client.do_request(url, lj_parser_recurse())
	ljpoolsize.append(url)


http_client.do_request("http://shared_lj.livejournal.com/data/foaf", lj_parser_recurse())
asyncore.loop()


import atomstream
import feedparser
from itertools import imap



while 1:
    for update in imap(feedparser.parse, atomstream.connect()):
        login=re.findall("http://(.+)\.livejournal\.com",update.feed.link)
        if login and login[0] != "community" and  (ljuser.get(login[0],None) ==  None):
    	    ljuser[login[0]]=1
            http_client.do_request("http://"+login[0]+".livejournal.com/data/foaf", lj_parser_recurse())
	    asyncore.loop()

write()