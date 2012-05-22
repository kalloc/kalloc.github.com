#!/usr/bin/python
# -*- coding: utf8 -*-
import sys,time,random,string,re,codecs,urllib,cStringIO,os,getopt,string
from threading import Thread 
from pycurl import *
from lxml.html import tostring as toHTML,fromstring as fromHTML
from random import choice
from urllib import urlencode
from urlparse import urlparse

import re

ReDom = re.compile("^(http://)?([^.]+\.)?([a-zA-Z]+)\.([a-zA-Z]{2,5})(/.*)?")

urlFile=False
comFile=False
opts, args = getopt.getopt(sys.argv[1:], 'u:vc:v',["url=","comment="])
for opt, val in opts:
    if opt in ("-u","--url"):
        urlFile = val
    elif opt in ("-c","--comment"):
        comFile = val

if not urlFile or not comFile:
    print """
\t --url файл_с_урлами.txt
\t --comment файл_с_комментарием.txt
\t -h это сообщение
"""
    sys.exit(0)



class Fetcher (Thread):
    url_retries={}
    urls=[]
    pattern=[]
    isRun=True
    conn=400
    timeout=30


    def initCurlMulti(self):
	m = CurlMulti()
	self.num_processed=0
	m.handles = []
	for i in range(self.conn):
    	    c = Curl()
	    c.setopt(FOLLOWLOCATION,0)
    	    c.setopt(MAXREDIRS, 5)
	    c.setopt(TIMEOUT,int(self.timeout*1.5))
	    m.handles.append(c)
	self.worker=m.handles[:]
	self.multi=m
    
    def __init__(self,main=False):
	
	if main is not False:
	    self.main=main
	    self.isRun=main.isRun
	    self.main.fetch=self
	    self.timeout=main.cfg.timeout
	else:
	    def loger(*strings):
	        if type(strings) == str:
        	    save=strings
        	else:
	    	    save=" ".join(map(str,strings))
	    	print "["+time.ctime()+"] "+save


	    self.loger=loger

	self.initCurlMulti()

	if main is not False:
	    Thread.__init__(self)
	    self.start()
	
	

    def defaultCallback(self,curl):
	pass

    def addPattern(self,pattern=('','')):
	if type(pattern) is 'dict':
	    [self.addPattern(pattern) for pattern in pattern]
	elif pattern == ('',''):
	    raise "не верный формат"
	else:
	    self.pattern.insert(0,(re.compile(pattern[0]),pattern[1]))
	return self

    def Callback(self,curl):
        curl.data=curl.res.getvalue()
	for m,call in self.pattern:
	    if re.match(m,curl.url):
		call(curl)
		return
	self.defaultCallback(curl)


    def addUrl(self,url):
	self.loger("FETCH ADD:", url)
	self.urls.insert(0,url)

    def getRandomUrl(self):
	return self.urls and self.urls.pop(random.randrange(len(self.urls))) or {}

    def getRandomIP(self,curl):
	pass

    def getCookie(self,curl):
	pass

	
    def createHandler(self):
	if not self.worker: return
	Url=self.getRandomUrl()
	if Url == {}: return
	url=Url["url"]
	cookie=self.getCookie(url)
	
	self.num_processed+=1
	c = self.worker.pop()
	c.url = url
	if not c.url in self.url_retries:
	    self.url_retries[c.url] = 2

	c.res = cStringIO.StringIO()
	c.head = cStringIO.StringIO()
	c.setopt(WRITEFUNCTION,c.res.write)
	c.setopt(HEADERFUNCTION,c.head.write)
	c.setopt(URL,url)
	if "verify" in Url:
	    c.verify=True
	else:
	    c.verify=False
	if not "verify" in Url and "post" in Url:
	    c.setopt(POST, 1)
	    c.setopt(POSTFIELDS,Url['post'])
	    c.save=Url
	else:
	    c.setopt(POST, 0)
	if not "verify" in Url and not "post" in Url:
	    c.post=False
	    c.save=Url

	
	self.multi.add_handle(c)


    def removeHandler(self,curl):
	#освбождаем хандлер
	self.multi.remove_handle(curl)
	self.worker.append(curl)
	self.num_processed-=1

    def process(self):
	if len(self.urls) == 0: return
	
	self.num_processed=0

	while len(self.urls) > 0 or self.num_processed > 0:
	    self.createHandler()

	    while 1:
		ret,num_h = self.multi.perform()
		if ret != E_CALL_MULTI_PERFORM: break

	    while 1:
		num_q, ok_list, err_list = self.multi.info_read()
		for c in ok_list:
		    self.removeHandler(c)
		    self.loger("FETCH OK:", urllib.unquote(c.getinfo(EFFECTIVE_URL)))
		    #вызываем калбэк
		    self.Callback(c)

		for c, errno, errmsg in err_list:
		    self.removeHandler(c)

		    #делаем попытку перекачать данные
		    if self.url_retries[c.url] > 0:
			self.loger("FETCH FAIL:",urllib.unquote(c.getinfo(EFFECTIVE_URL)),errno,errmsg,self.url_retries[c.url])
	    	        self.addUrl(c.save)
	    	        self.url_retries[c.url]-=1
	    	    else:
			self.loger("FETCH FAIL:",urllib.unquote(c.getinfo(EFFECTIVE_URL)),errno,errmsg)

		if num_q == 0 and len(err_list) == 0:
		    break
	    try:
		self.multi.select(1.0)
	    except:
		pass


    def run(self):
	while self.isRun:
	    self.process()

class AntiCaptcha:
    def __init__(self):
	pass
    def send(self,cap):
	pass
    def check(self,cap):
	pass
	
if __name__ == "__main__":
    def readUrls(num=10):
	fpUrls=open(urlFile)
	while True:
	    readed = num
	    ret=[]
	    try:
		while readed:
		    readed=readed-1
		    ret.append(fpUrls.next().strip())
	    except:
		fpUrls.close()
		pass
	    yield ret

    fpCom=open(comFile)
    comments=[x.strip() for x in fpCom.readlines()]

    def genName(size=10):
	return ''.join([choice(string.lowercase) for i in range(size)])


    def genURL(zone="com"):
	return "http://"+genName(20)+"."+genName(8)+"."+zone+"/"
    
    def genEmail(zone="com"):
	return genName(20)+"@"+genName(8)+"."+zone
    def getRealAction(url,action):
	if action.startswith('http://'):
	    return action
	u = urlparse(url)
	
	if action.startswith('/'):
	    return o.scheme+'://'+o.netloc+action
	else:
	    return o.scheme+'://'+o.netloc+'/'.join(o.path.split("/")[:-1])+'/'+action
	


    def parse(x):
	if "post" in x.save:
	    header=dict([xx.strip().split(': ') for xx in filter(lambda x: x.find(":") > 0,x.head.getvalue().split("\n"))])
	    header=dict(map(lambda h:[h.lower(),header[h]],header))
	    if "location" in header and header["location"].find("comment"):
		print "забеись",x.save["original"],x.save["comment"]
	    else:
		print "плохо",x.save["url"],header,x.data

	else:
	    if  len(x.data) == 0:
		return
	
	    dom=fromHTML(x.data) 
	    if len(dom) == 0:
		return
	
	    form=filter(lambda x: x.get("action") and x.get("action").find("wp-comments-post.php"),dom.xpath("//form[@method='post']"))[0].forms[0]
	    fields =  form.form_values()
	    commentName=False
    	    for field in fields:
		if field[0] in ('author','login','name'):
		    form.fields[field[0]]=genName()
		elif field[0] in ('email','e-mail','mail'):
		    form.fields[field[0]]=genEmail()
		#elif field[0] in ('site','url'):
	#		form.fields[field[0]]=genURL()
	        elif field[0] in ('comment','text','body'):
		    commentName=field[0]
	    submit= filter(lambda x: x.get('type') == "submit",form.inputs)[0]
	    for comment in comments:
		form.fields[commentName]=comment
	        fetch.addUrl({"url":getRealAction(x.url,form.action),"post":urlencode(form.form_values()+[(submit.name,submit.value)]),"original":x.url,"comment":comment})

    fetch=Fetcher()
    fetch.addPattern(('.*',parse))
    
    try:
	urlsIter=readUrls(1)
        while fetch.isRun:
    	    urls=urlsIter.next()
    	    if not urls: fetch.isRun=0
    	    for url in urls:
    		fetch.addUrl({"url":url,"original":url})
	    while fetch.urls and fetch.num_processed == 0: 
	    	fetch.process()
	    
	    
    except KeyboardInterrupt:
	fetch.isRun=0

