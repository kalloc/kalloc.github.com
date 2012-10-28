#!/usr/bin/python
# -*- coding: utf8 -*-
import sys,time,random,string,re,codecs,urllib,cStringIO,os
from threading import Thread 
from pycurl import *

host=""
if len(sys.argv) < 2:
    print "usage %s host " %(sys.argv[0])
    sys.exit()
host=sys.argv[1]
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
	self.loger("FETCH ADD:", url[0])
	self.urls.insert(0,url)

    def getRandomUrl(self):
	return self.urls and self.urls.pop(random.randrange(len(self.urls))) or (False,False)

    def getRandomIP(self,curl):
	pass

    def getCookie(self,curl):
	pass

	
    def createHandler(self):
	if not self.worker: return
	Url=self.getRandomUrl()
	url,key=Url
	if url is False: return
	
	cookie=self.getCookie(key)
	
	self.num_processed+=1
	c = self.worker.pop()
	c.url = url
	c.key = key
	if not c.url in self.url_retries:
	    self.url_retries[c.url] = 2

	c.res = cStringIO.StringIO()
	c.setopt(WRITEFUNCTION,c.res.write)
	c.setopt(URL,url)
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
	    	        self.addUrl((c.url,c.key))
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
    def parse_svn(x):
	if x.url.find("templates_c") != -1:
	    return
	FileS=re.findall("(.+)\n(file|dir)\n",x.data)
	Host=x.url.replace("http://","").replace(".svn/entries","").replace(".svn-base","").split("/")[0]
	Path="/"+"/".join(x.url.replace("http://","").replace(".svn/entries","").replace(".svn-base","").split("/")[1:])
	try:
	    os.stat("/download/"+Host+Path)
	except:
	    os.mkdir("/download/"+Host+Path)
	if FileS:
	    for File,Type in FileS:
		if Type == "dir":
		    fetch.addUrl(('http://'+Host+Path+'/'+File+'/.svn/entries',Host))
		else:
		    if not re.findall("(avi|flv|pot|mo|po|txt|htm|tpl|ico|swf|js|gif|css|png|jpg|html)$",File):
			fetch.addUrl(('http://'+Host+Path+'/.svn/text-base/'+File+'.svn-base',Host))
	
    def save_svn(x):
	if len(x.data) == 0: return
	Host=x.url.replace("http://","").replace(".svn/text-base","").replace(".svn-base","").split("/")[0]
	Path="/".join(x.url.replace("http://","").replace(".svn/text-base/","").replace(".svn-base","").split("/")[1:])
	fp=file("/download/"+Host+Path,"w")
	fp.write(x.data)
	fp.close()

    fetch=Fetcher()
    fetch.addPattern(('.+\.svn/entries$',parse_svn))
    fetch.addPattern(('.+\.svn-base$',save_svn))
    try:
	os.stat("/download/"+host)
    except:
        os.mkdir("/download/"+host)

    fetch.addUrl(('http://'+host+'/.svn/entries',host))
    
    try:
	while fetch.urls and fetch.num_processed == 0: 
	    fetch.process()
    except KeyboardInterrupt:
	fetch.isRun=0

