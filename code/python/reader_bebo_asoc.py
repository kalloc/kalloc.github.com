#!/usr/bin/python
# -*- coding: utf8 -*-

import http_client_proxy, asyncore
import string
import re
import codecs
import csv

def sort_by_value(d):
    """ Returns the keys of dictionary d sorted by their values """
    items=d.items()
    backitems=[ [v[1],v[0]] for v in items]
    backitems.sort()
    return [ backitems[i][1] for i in range(0,len(backitems))]

def info(v):
     return '%s = %r %s' % (v, v, type(v))

bebouser = []
bebopool=[]
bebopoolsize=[]

class parser:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        if (client.status[1] == "301"):
	    http_client_proxy.do_request(header['location'], parser())
	else:
    	    print self.host + self.path +" "+ client.status[0] + " " + client.status[1]

    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self,client): 
	if not self.data:
	    getit()
	    return
	logins=re.findall("""<a href=/Profile.jsp\?MemberId=([0-9]+)>""",self.data)
	for login in logins:
	    bebouser.append([login])

	next=re.findall("""<a href='(\?SearchType=[^']+)'>Next &gt;&gt;</a>""",self.data)
	if(next):
	    bebopool.append("http://www.bebo.com/Search2.jsp"+next[0])

	if(len(bebouser) >= 50):
            writer.writerows(bebouser)
	    for i in range(0,len(bebouser)):
		bebouser.pop()

        bebopoolsize.pop()
	getit()
	return 



URLS = (
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=home",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=me",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=my",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=you",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=for",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=im",
    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=right",
)


def getit():
    while len(bebopoolsize)<1 and len(bebopool)>0:
        url=bebopool.pop()
        http_client_proxy.do_request(url, parser())
        bebopoolsize.append(url)
				    

filename = "bebologin.txt"
writer = csv.writer(open(filename, "ab"),dialect=csv.excel)

for url in URLS:
    bebopool.append(url)


getit()

asyncore.loop()


writer.writerows(bebouser)


