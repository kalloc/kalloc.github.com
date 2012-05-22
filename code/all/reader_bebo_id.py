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
			try:
			    mask[0](c)
			except:
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
    logins=re.findall("""<a href=/Profile.jsp\?MemberId=([0-9]+)><b""",self.data)
    for login in logins:
	if not login in user:
	    query.append("http://www.bebo.com/Profile.jsp?MemberId="+login)
	    user.append(login)


    term=re.findall("""\?SearchType=People&SearchTerm=([^&]+)&""",self.url)
    if  re.findall("s.bebo.com",self.data) and len(logins) == 0:
    
	if term[0] in count_error and  count_error[term[0]] > 50:
	    url="http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm="+term[0]
	    while url in URLS: URLS.remove(url)
	    print "Remove url",self.url
	    
	else:
	    print "Error url",self.url
	    query.append(self.url)
	    count_error[term[0]]=term[0] in count_error and count_error[term[0]]+1 or 1
    elif not re.findall("s.bebo.com",self.data):
	query.append(self.url)
    else:
	count_error[term[0]]=0

    return 

def parser_user(self): 


    says=None
    gender=0
    comment=None
    interes=[]

    row=[]

    doc = fromstring(self.data)

    id=re.findall("mail/MailCompose.jsp\?ToMemberId=([0-9]+)",self.data)

    if not id:
	if not re.findall("s.bebo.com",self.data):
	    query.append(self.url)
	return

    id=id[0]


    says=None
    gender=0
    comment=None
    interes=[]
    row=[]

    #izvlekaem gender
    tables=doc.xpath("//table[@class='top_font']")
    gender=tostring(tables[1].xpath("tr/td")[3],method="text")
    gender=gender == "Female" and 2 or 1


    #izvlecaem id
    id=re.findall("mail/MailCompose.jsp\?ToMemberId=([0-9]+)",self.data)[0]

    #izvlekae name,login
    title=tostring(doc.xpath("//title")[0],method="text")
    name=title[0:title.rindex("<")-1]
    login=title[title.rindex("<")+1:title.rindex(">")-1]

    #izvlekaem comment
    p=doc.xpath("//td[@class='top_font']/p")
    comment=tostring(p[0],encoding="utf8",method="text").replace("\n"," ").replace("\r","")

    #izvlekaem says
    says=tostring(doc.xpath("//td[@class='top_font']")[0],method="text",encoding="utf8")
    if says.find(" says:") > 0:
        says=says[says.index(" says:")+9:says.rindex("(")-2]
        says=says.replace("\n"," ").replace("\r","")
    else:
        says=None

    top=[id,name,login,gender,says,comment]
    before=[id,name,login,gender]

    #figachim interes
    if tostring(tables[0],method="text").find("Member Since") == -1: tr=tables[0].xpath("tr")
    else: tr=tables[1].xpath("tr")
    for i in range(0,len(tr),3): 
        interes.append(before+[tostring(tr[i],encoding="utf8",method="text").replace("\n"," ").replace("\r",""),tostring(tr[i+1],encoding="utf8",method="text").replace("\n"," ").replace("\r","")])


    #print "TO -> userinfo.txt:",top
    #for one in interes:
    #    print "TO -> interes.txt:",one


    fp_writer.writerow(top)
    fp_interes.writerows(interes)


    return 





filename = "bebologin.txt"
try:
    fp = open(filename,'r')
except:
    fp = 0

if  fp != 0:
    fp.close()
    reader = csv.reader(open(filename, "r"),dialect=csv.excel)
    try:
	for row in reader:  user.append(row[0])
    except:
	print row


fp_writer = csv.writer(open(filename, "ab"),dialect=csv.excel)

fp_interes = csv.writer(open("bebointeres.txt", "ab"),dialect=csv.excel)





def getit():
    while len(query) >0:
	url=[]
	while len(query)>0:
	    url.append(query.pop())
	del query[:]
	fetch(url,[[parser,"Search2.jsp"],[parser_user,"Profile.jsp"]],len(proxy_list)/2)

URLS = [
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=home",
#    "http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=my",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=you",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=me",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=im",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=right",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=i",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=a",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=ju",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=wa",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=thi",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=friend",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=it",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=year",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=u",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=mis",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=be",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=mo",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=alway",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=al",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=ha",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=mu",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=that",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=mate",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=thing",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=my",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Thi",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=eye",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=per",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=hi",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=el",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=la",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=A",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=hou",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=girl",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=re",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=si",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=kid",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=month",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=fir",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Ju",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=becau",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=m",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=I",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=di",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=time",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=day",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=name",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=ea",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Brushe",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Choo",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=tho",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=boy",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=li",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=guy",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Mis",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Statu",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=e",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=See",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=ba",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=doe",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Chri",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Alway",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=lot",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=cour",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=fini",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=m8",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=week",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=mes",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=wi",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=plea",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=cau",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=bu",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=It",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=de",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=load",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=in",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=as",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=lo",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Stand",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=love",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=awe",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=pas",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=ye",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=pa",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=parent",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=sometime",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=vi",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Au",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=pis",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=gues",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=co",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Friend",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Love",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=the",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=Eye",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=clo",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=weekend",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=hu",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=goe",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=sport",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=nur",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=make",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=con",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=sister",
"http://www.bebo.com/Search2.jsp?SearchType=People&SearchTerm=know",
]



offset=0


while len(URLS)>0:
    query=[]
    for offset in range(offset,offset+150):
	for url in URLS:
	    query.append(url+"&PageNum="+str(offset+1)+"&FirstResult="+str(offset*10))
    getit()










