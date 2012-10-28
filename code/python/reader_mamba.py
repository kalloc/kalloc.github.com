#!/usr/bin/python
# -*- coding: utf8 -*-

import sys 
import pdb
from pycurl import *
import cStringIO
import urllib,time,random,socket
from lxml.html import fromstring,tostring
import string
import re
import os
import codecs
import csv


offset=0
mambauser={}
mambapool=[]
mambaquery=[]
counter_exists=[0]
counter=[0]


socket.setdefaulttimeout(2)

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
	c.setopt(HTTPHEADER, ['Cookie: saved_city_id=4400; TIMEZONE_UPDATE=1210415288; UID=206713706; SECRET=ptOS26; LEVEL=Low; LOGIN=porosenok_20080; s=vKxo6x1qtmeOQ92NNndcFoF2S7bY1nQk','User-Agent: Mozilla/5.0 (rv:1.8.1.12) Gecko/20080129 Firefox/2.0.0.14 Windows; U; Windows NT 6.0; en-US; rv:1.8.1.14'])
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
	    m.add_handle(c)
	    c.proxy=proxy_list[prx_num]
	    c.url=url
	while 1:
	    ret,num_h = m.perform()
	    if ret != E_CALL_MULTI_PERFORM: break

	    
        while 1:
            num_q, ok_list, err_list = m.info_read()
            for c in ok_list:
                m.remove_handle(c)
		c.data=unicode(c.res.getvalue(),'utf-8')
#                print "FETCH OK:", c.getinfo(EFFECTIVE_URL),
                worker.append(c)
		for mask in func:
		    if re.findall(mask[1],c.url):
#			print " open by mask",mask[1]
			mask[0](c)
            for c, errno, errmsg in err_list:
                m.remove_handle(c)
		c.data=c.res.getvalue()
#                print "FETCH FAIL:",c.getinfo(EFFECTIVE_URL),errno,errmsg
                pool.append(c.url)
                worker.append(c)
            num_processed = num_processed + len(ok_list)
            if num_q == 0:
                break
        m.select(1.0)


    return

#fetch(mambapool,parse,10)


def mamba_parser(self):

    counter_exists[0]=0
    counter[0]=0

    offset=re.findall("""offset=([0-9]+)""",self.url)

    doc = fromstring(self.data)
    #izvlekaem logins
    users=doc.xpath("//tr[@class='user']")
    if users:

        for user in users:
    	    user=tostring(user,encoding="utf-8")
    	    
    	    login=re.findall("""<a href="http://mamba.ru/([^/]+)/""",user)
    	    anketa=re.findall(""";anketaId=([^"]+)">([^<]+)</a></span>, ([^<]+)</b>""",user)
	    gender=re.findall("""http://[^/]+/images/default/default/ico_(f|m).gif""",user)
	    
	    location=re.findall("""<span class="greyer">([^,]+), ([^<]+)</span>""",user)


	    if not gender:
    		mambaquery.append("http://mamba.ru/"+login+"/?1=1&afolder=albums")
		sys.stdout.write("похоже надо ввести каптчу\a\b\n")
	        time.sleep(5)
    		return

	    name=anketa[0][1]
	    age=anketa[0][2]
	    userid=int(anketa[0][0])
	    login=login[0]
	    country=location[0][0] or location[0][0] and ""
	    city=location[0][1] or location[0][1] and ""
	    gender=gender[0][0] == "m" or 1 and 2

	    mambapool.append([userid,login,name,gender,age,country,city])
            if login and  (mambauser.get(login,None) ==  None):
	        mambauser[login]=1
	        mambaquery.append("http://mamba.ru/photo/?anketa_id="+str(userid)+"&albumId="+str(userid+1))
	        counter[0]=int(counter[0]) + 1
	    else:
    	        counter_exists[0]=int(counter_exists[0]) + 1
    print "FETCH INDEX:  offset: "+ str(offset[0]) + ", read:",len(self.data),", add: ",counter[0],"already exists: ",counter_exists[0],"in pool write:",len(mambapool),"total:",len(mambauser)

    

    write()

    if counter[0] == 0 and counter_exists[0] == 0:
	if not re.match("""corp.mamba.ru""",self.data):
	    mambaquery.append(self.url)
	else:
    	    sys.exit()
    return 




def mamba_photo_parser(self):

    user_id=""
    pic_id=""
    huges=re.findall("""(http://[^/]+/[0-9]+/[0-9]+/[0-9]+/([0-9]+)/([0-9]+)_huge.jpg\?cc=[a-z0-9]+)'""",self.data)
    if not huges:
	print "FALSE Load from:",self.url
	return
    print "TRUE Load from:",self.url
    if(len(huges)==8):
	offset=re.findall("""&offset=([0-9]+)""",self.url)
	if offset:
	    mambaquery.append(self.url.replace("offset="+offset[0],"offset="+str(int(offset[0])+8)))
	else:
	    mambaquery.append(self.url + "&offset=8")
    medium=""
    user_id=huges[0][1]

    smalls=re.findall("""(http://[^/]+/[0-9]+/[0-9]+/[0-9]+/([0-9]+)/([0-9]+)_small.jpg\?updated=[0-9]+)""",self.data)
    #small=re.findall("""'(http://[^/]+/[0-9]+/[0-9]+/[0-9]+/"""+user_id+"""/"""+pic_id+"""_small.jpg\?updated=[^"]+)'""",self.data)

    try:
        os.mkdir("mamba/"+user_id,0777)
        path="mamba/"+user_id+"/"
    except:
        path="mamba/"+user_id+"/"

    for huge in huges:
	try: 
	    user_id=huge[1]
	    pic_id=huge[2]
	    huge=huge[0].encode("utf-8")
	    fp=open(path+pic_id+"_huge.jpg","wb")
	    fp.write(urllib.urlopen(huge).read())
	    fp.close()
	    print "FETCH Photo",huge
	except: 
	    print "ERROR FETCH Photo",huge
	    pass


    for small in smalls:
	try: 
	    user_id=small[1]
	    pic_id=small[2]
	    small=small[0].encode("utf-8")
    	    medium=small.replace("_small","_medium")
	    fp=open(path+pic_id+"_small.jpg","wb")
	    fp.write(urllib.urlopen(small).read())
	    print "FETCH Photo",small
	    fp.close()
	except:
	    print "ERROR FETCH Photo",small
	    pass

	try: 
	    medium=small.replace("_small","_medium")
	    fp=open(path+pic_id+"_medium.jpg","wb")
	    fp.write(urllib.urlopen(medium).read())
	    fp.close()
	    print "FETCH Photo",medium
	except: 
	    print "ERROR FETCH Photo",medium
	    pass

    write()

    return 





def mamba_user_parser(self):
    login=re.findall("""http://mamba.ru/([^/]+)/""",self.url)
    if not login:
	print self.url
	return
    login=login[0]

    if re.findall("""Эта страница не заполнена, отклонена модератором или не разрешена владельцем для просмотра""",self.data):
        return

    gender=re.findall("""<img src="http://[^/]+/images/default/default/ico_(f|m).gif" width=11 height=11 border=0""",self.data)
    user_id=re.findall("""my/message.phtml\?action=Message\&oid=[0-9]+\&uid=([0-9]+)""",self.data)
    title=re.findall("""<title>([^,]+), ([^,]+), ([^,]+), ([^\s]+)""",self.data,re.M)
    photo=re.findall("""(http://mamba.ru/photo/\?anketa_id=[0-9]+\&oid=[0-9]+\&albumId=[0-9]+)""",self.data)

    if not gender:
        mambaquery.append("http://mamba.ru/"+login+"/?1=1&afolder=albums")
	sys.stdout.write("похоже надо ввести каптчу\a\b\n")
        time.sleep(5)
        return
    title=title[0]
    name=title[0] or title[0] and ""
    age=title[1] or title[1] and ""
    country=title[2] or title[2] and ""
    city=title[3] or title[3] and ""
    

    if gender[0] == "m" :
        gender=1
    elif gender[0] == "f" :
        gender=2
	
    photoz=[]

    if photo:
        for ph in photo:
	    if not ph in  photoz:
	        photoz.append(ph)
		mambaquery.append(ph)
	mambapool.append([user_id[0],login,name,gender,age,country,city])

    write()
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
	if os.path.exists("mamba/"+row[0]):
	    mambauser[row[1]]=1
    


writer = csv.writer(open(filename, "ab"),dialect=csv.excel)



def write():
    if len(mambapool)>0:
	writer.writerows(mambapool)
	del mambapool[:]



def getit():
    if len(mambaquery) >0:
	url=mambaquery[:]
	del mambaquery[:]
	fetch(url,[[mamba_parser,"/search.phtml"],[mamba_photo_parser,"/photo/"],[mamba_user_parser,"/[^/]+/\?1=1\&afolder=albums$"]],30)

offset=0

while True:
    for offset in range(offset,offset+50,10):
	mambaquery.append("http://mamba.ru/search.phtml?offset="+str(offset)+"&s_l=F&s_i=M&s_f=18&s_t=25&s_c=3159_4312_4400&s_m=0&s_p=checked&s_w=checked&s_ty=All&s_vf=&s_vw=&s_vip=&s_v=&search_mode=Base&gid=260234362")
    getit()
