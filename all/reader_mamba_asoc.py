#!/usr/bin/python
# -*- coding: utf8 -*-

import http_client_proxy, asyncore,socket
import urllib
import time
import string
import re
import os
import codecs
import csv


mambauser={}
mambapool=[]
mambaquery=[]
mambapoolsize=[]
offset=[]

socket.setdefaulttimeout(1)

def isset(where,who):
    try:
	return getattr(where,who)
    except:
    	return 0

class mamba_parser_recurse:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        #print "RECURSE GET",self.host + self.path ,client.status[0] , client.status[1]
        if (client.status[1] == "301" or client.status[1] == "302"):
	    http_client_proxy.do_request(header['location'], mamba_parser_recurse())

    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self,client): 

	login=client.path.replace("/","")

	if not self or isset(self,"data") == 0 or isset(self,"header") == 0:
	    return
	seealso=re.findall("""<b><span dir=ltr><a href="http://mamba.ru/([^/]+)/[^"]+">([^<]+)</a></span>""",self.data)
	
	counter=0
	counter_exists=0
        offset=re.findall("""offset=([0-9]+)""",self.path)


	if seealso:
	    for login in seealso:
	        if login and  (mambauser.get(login[0],None) ==  None):
		    print login[0],
		    mambauser[login[0]]=1
		    mambaquery.append("http://mamba.ru/"+login[0]+"/")
		    counter+=1
		else:
    		    counter_exists+=1
		    
	    offset_old=int(offset[0])
    	    offset=offset_old+10
	    
	    print "\n"+self.host + " offset: "+ str(offset_old) + ", read:",len(self.data),", add: ",counter,"already exists: ",counter_exists

        if len(mambapool) > 1:
	    write()
	if len(mambapoolsize) < 1 and len(mambaquery) > 0:
	    getit()
	
	if len(mambapoolsize) > 0:
	    mambapoolsize.pop();	

	return 




class mamba_photo_parser:
    data = ""
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        print "PHOTO GET",self.host + self.path ,client.status[0] , client.status[1]
        if (client.status[1] == "301" or client.status[1] == "302"):
	    http_client_proxy.do_request(header['location'], mamba_photo_recurse())
	if (client.status[1] != "200"):
	    http_client_proxy.do_request(header['location'], mamba_photo_recurse())
	    return

    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self,client): 
	login=client.path.replace("/","")

	if not self or isset(self,"data") == 0 or isset(self,"header") == 0:
	    return
	huge=re.findall("""id="iphoto_huge" src="(http://[^/]+/[0-9]+/[0-9]+/[0-9]+/([0-9]+)/([0-9]+)[^"]+)""",self.data)
	if not huge:
	    print huge
	    return
	user_id=huge[0][1]
	pic_id=huge[0][2]

	small=re.findall("""(http://[^/]+/[0-9]+/[0-9]+/[0-9]+/"""+user_id+"""/"""+pic_id+"""_small.jpg\?updated=[^"]+)""",self.data)

	try:
	    os.mkdir("/mamba/"+user_id,0777)
	    path="/mamba/"+user_id+"/"
	except:
	    path="/mamba/"+user_id+"/"


	huge=huge[0][0]
	fp=open(path+pic_id+"_huge.jpg","wb")
	fp.write(urllib.urlopen(huge).read())
	fp.close()
	print huge



	small=small[0]
	fp=open(path+pic_id+"_small.jpg","wb")
	fp.write(urllib.urlopen(small).read())
	fp.close()
	print small


	medium=small.replace("_small","_medium")
	fp=open(path+pic_id+"_medium.jpg","wb")
	fp.write(urllib.urlopen(medium).read())
	fp.close()
	print medium


	
        if len(mambapool) > 10:
	    write()
	if len(mambapoolsize) < 50 and len(mambaquery) > 0:
	    getit()

	if len(mambapoolsize) > 0:
	    mambapoolsize.pop();	

	return 





class mamba_user_parser:
    def http_header(self, client):
        self.host = client.host
        self.header = client.header
        self.path = client.path
        self.data = ""
        header = {}
        header = dict([(l[0], l[1]) for l in client.header.items()])
        if (client.status[1] == "301" or client.status[1] == "302"):
	    http_client_proxy.do_request(header['location'], mamba_user_recurse())
	    return
	if (client.status[1] != "200"):
	    http_client_proxy.do_request(header['location'], mamba_user_recurse())
	    return
	
    def http_failed(self, client):
        print self.host, "failed"

    def readit(self, data):
	self.data=self.data+data
	return

    def close(self,client): 
	login=client.path.replace("/","")
	if not self or isset(self,"data") == 0 or isset(self,"header") == 0:
	    mambaquery.append("http://mamba.ru/"+login+"/")
	    return

	if re.findall("""Эта страница не заполнена, отклонена модератором или не разрешена владельцем для просмотра""",self.data):
	    return

        print "USER GET",client.host + client.path ,client.status[0] , client.status[1]
        gender=re.findall("""<img src="http://[^/]+/images/default/default/ico_(f|m).gif" width=11 height=11 border=0""",self.data)
        user_id=re.findall("""my/message.phtml\?action=Message\&oid=[0-9]+\&uid=([0-9]+)""",self.data)
        title=re.findall("""<title>([^,]+), ([^,]+), ([^,]+), ([^\s]+)""",self.data,re.M)
        photo=re.findall("""(http://mamba.ru/photo/\?anketa_id=[0-9]+\&oid=[0-9]+\&albumId=[0-9]+)""",self.data)

	if not gender:
	    mambaquery.append("http://mamba.ru/"+login+"/")
	    print self.data
	    time.sleep(10)
	    return
	title=title[0]
	name=title[0] or title[0] and ""
	age=title[1] or title[1] and ""
	country=title[2] or title[2] and ""
	city=title[3] or title[3] and ""
	

	if gender[0] == "m" :
	    gender=1
	else:
	    if gender[0] == "f" :
		gender=2
	
	photoz=[]

	if photo:
	    for ph in photo:
		if not ph in  photoz:
		    photoz.append(ph)
		    mambaquery.append(ph)
	    mambapool.append([user_id[0],login,name,gender,age,country,city])

        if len(mambapool) > 1:
	    write()
	if len(mambapoolsize) < 1 and len(mambaquery) > 0:
	    getit()

	if len(mambapoolsize) > 0:
	    mambapoolsize.pop();	

	return 


filename = "mambalogin.txt"
try:
    fp = open(filename,'r')
except:
    fp = 0

if  fp != 0:
    fp.close()
    reader = csv.reader(open(filename, "r"),dialect=csv.excel)
    for row in reader:
        mambauser[row[1]]=1
    


writer = csv.writer(open(filename, "ab"),dialect=csv.excel)



def write():
    writer.writerows(mambapool)
    del mambapool[0:len(mambapool)]



def getit():
    while len(mambapoolsize)<50 and len(mambaquery) >0:

	url=mambaquery.pop()

	if url.find("/photo/") > 0:
	    http_client_proxy.do_request(url, mamba_photo_parser())
	else:
	    http_client_proxy.do_request(url, mamba_user_parser())

	mambapoolsize.append(url)
	time.sleep(1)

offset=0
while True:
    try:
	http_client_proxy.do_request("http://mamba.ru/search.phtml?offset="+str(offset)+"&s_l=F&s_i=M&s_f=18&s_t=25&s_c=3159_4312_4400&s_m=0&s_p=checked&s_w=checked&s_ty=All&s_vf=&s_vw=&s_vip=&s_v=&search_mode=Base&gid=260234362", mamba_parser_recurse())
        asyncore.loop()
	offset=offset+10
    except:
	offset=offset-10
