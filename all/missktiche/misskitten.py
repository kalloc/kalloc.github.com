#!/usr/bin/python
# -*- coding: utf8 -*-
import sys,time,random,string,re,codecs,urllib,cStringIO,os
from threading import Thread 
from lxml.html import tostring as toHTML,fromstring as fromHTML
from pycurl import *

class Fetcher (Thread):
    url_retries={}
    urls=[]
    pattern=[]
    isRun=True
    conn=100
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
        return self.urls and self.urls.pop(random.randrange(len(self.urls))) or False

    def getRandomIP(self,curl):
        pass

    def getCookie(self,curl):
        pass

        
    def createHandler(self):
        if not self.worker: return
        Url=self.getRandomUrl()
        url=Url
        if url is False: return

        self.num_processed+=1
        c = self.worker.pop()
        c.url = url
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
                    try:
                        self.Callback(c)
                    except:
                        pass

                for c, errno, errmsg in err_list:
                    self.removeHandler(c)

                    #делаем попытку перекачать данные
                    if self.url_retries[c.url] > 0:
                        self.loger("FETCH FAIL:",urllib.unquote(c.getinfo(EFFECTIVE_URL)),errno,errmsg,self.url_retries[c.url])
                        self.addUrl(c.url)
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


if __name__ == "__main__":
    fp=open("misskitten.log",'a')
    def save(string):
        print type(string),string
        fp.write(string.encode('utf-8')+"\n")
        fp.flush()
    def parse_page(x):
        dom=fromHTML(x.data.decode('cp1251')) 
        if len(dom) == 0:
            return
        for msg in dom.xpath("/html/body/font")[0].text_content().split("\n"):
            if msg.find("<misskitten>") > 0:
                 save("".join(msg.split("> ")[1:]))
        for uri in [el.get("href") for el in dom.xpath("/html/body/a")]:
             if uri.startswith("/viewlog/") and int(x.url.split("=")[-1])+1 == int(str(uri).split("=")[-1]):
                  fetch.addUrl('http://logs.zub.cc'+uri)
        pass

    def parse_year(x):
        dom=fromHTML(x.data) 
        if len(dom) == 0:
             return
        for uri in  [x.get("href") for x in dom.xpath("//table/tr/td/a")]:
             fetch.addUrl('http://logs.zub.cc'+uri)
        pass


    fetch=Fetcher()
    fetch.addPattern(('.+&page=([0-9]+)$',parse_page))
    fetch.addPattern(('.+?year=([0-9]+)$',parse_year))

    fetch.addUrl('http://logs.zub.cc/moskwa/?year=2010')
    fetch.addUrl('http://logs.zub.cc/moskwa/?year=2009')
    fetch.addUrl('http://logs.zub.cc/moskwa/?year=2008')
    try:
        while fetch.urls and fetch.num_processed == 0: 
            fetch.process()
    except KeyboardInterrupt:
        fetch.isRun=0
    fp.close()

