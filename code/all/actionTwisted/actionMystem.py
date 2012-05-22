#!/usr/bin/python
# -*- coding: utf8 -*-


from config import Config
from twisted.python.logfile import DailyLogFile
from twisted.python.log import startLogging,err,failure
import traceback
from chardet import detect as cp
from twisted.web import server, resource , static
from twisted.internet import threads,reactor
from twisted.internet.defer import Deferred,DeferredList
from twisted.web.client import getPage as _getPage
from md5 import md5

import re
import sys
import signal
import re
import pty
import termios
import os
import logging
import time
import Queue
import StringIO
import urlparse
import threading
import StringIO
import simplejson as json
from lxml.html import fromstring,tostring
from lxml.html.clean import clean_html
try: # try importing the fast, C version
    from twisted.protocols._c_urlarg import unquote,quote
except ImportError:
    from urllib import unquote,quote


from twisted.internet import protocol
from twisted.internet import reactor
import re
import time
import copy



#cleaner = Cleaner(page_structure=False, links=False)
_re1 = re.compile("([^{]+){([^}]+)}").findall
_clean=re.compile(u"[\.\,\!\t\r\n \:\/\*\?\#\@\`\%\&\(\)_\+\|\[\]\-—\>\<\"\«\»\s]+",re.MULTILINE).sub
_stop=re.compile(u"(^|[\s]+)([0-9]+|й|и|в|во|не|что|он|на|я|с|со|как|а|то|все|она|так|его|но|да|ты|к|у|же|вы|за|бы|по|только|ее|мне|было|вот|от|меня|еще|нет|о|из|ему|теперь|когда|даже|ну|вдруг|ли|если|уже|или|ни|быть|был|него|до|вас|нибудь|опять|уж|вам|сказал|ведь|там|потом|себя|ничего|ей|может|они|тут|где|есть|надо|ней|для|мы|тебя|их|чем|была|сам|чтоб|без|будто|человек|чего|раз|тоже|себе|под|жизнь|будет|ж|тогда|кто|этот|говорил|того|потому|этого|какой|совсем|ним|здесь|этом|один|почти|мой|тем|чтобы|нее|кажется|сейчас|были|куда|зачем|сказать|всех|никогда|сегодня|можно|при|наконец|два|об|другой|хоть|после|над|больше|тот|через|эти|нас|про|всего|них|какая|много|разве|сказала|три|эту|моя|впрочем|хорошо|свою|этой|перед|иногда|лучше|чуть|том|нельзя|такой|им|более|всегда|конечно|всю|между)(?=\s+|$)").sub
stopwords = set(u"й|и|в|во|не|что|он|на|я|с|со|как|а|то|все|она|так|его|но|да|ты|к|у|же|вы|за|бы|по|только|ее|мне|было|вот|от|меня|еще|нет|о|из|ему|теперь|когда|даже|ну|вдруг|ли|если|уже|или|ни|быть|был|него|до|вас|нибудь|опять|уж|вам|сказал|ведь|там|потом|себя|ничего|ей|может|они|тут|где|есть|надо|ней|для|мы|тебя|их|чем|была|сам|чтоб|без|будто|человек|чего|раз|тоже|себе|под|жизнь|будет|ж|тогда|кто|этот|говорил|того|потому|этого|какой|совсем|ним|здесь|этом|один|почти|мой|тем|чтобы|нее|кажется|сейчас|были|куда|зачем|сказать|всех|никогда|сегодня|можно|при|наконец|два|об|другой|хоть|после|над|больше|тот|через|эти|нас|про|всего|них|какая|много|разве|сказала|три|эту|моя|впрочем|хорошо|свою|этой|перед|иногда|лучше|чуть|том|нельзя|такой|им|более|всегда|конечно|всю|между".split("|"))
#HTTP = 1
#PLAIN = 2
#URL = 4

def getPage(url): return _getPage(url,timeout=2,agent="Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; API.ADVPEOPLE.RU)" )




def clean(text,stop=False):
    #print "only clean",_clean(" ",text.lower())
    if stop:
        #return ' '.join(s for s in _clean(" ",text.lower()).split() if not s in stopwords and not s.isdigit())
        return _stop("",_clean(" ",text.lower()))
    return _clean(" ",text.lower())

def merge(total,add,condition=None):
    if not total:
        return copy.deepcopy(add)
    for i in range(len(total)):
        if type(total[i]) in [int,float]:
            total[i]=total[i]+add[i]
        else:
            total[i]=merge(total[i],add[i])
        return total

def compare(total,condition,add):
    if not total:
        return copy.deepcopy(add)
    for i in range(len(total)):
        if type(total[i]) in [int,float]:
            if (condition == '<' and add[i] < total[i]) or (condition == '>' and add[i] > total[i]):
                total[i]=add[i]
        else:
            total[i]=compare(total[i],condition,add[i])
    return total


def toHex(s):
    lst = []
    for ch in s:
        hv = hex(ord(ch)).replace('0x', '')
        if len(hv) == 1:
            hv = '0'+hv
        lst.append(hv)
    return reduce(lambda x,y:x+y, lst)


def getPath(file,Make=False,Root=True,Ext=".js",Depth=3):
    global root_path
    path=root_path if Root else ""
    for x in xrange(Depth):
        path+=(file[x] if file[x] != "." else "_") +"/"
        if Make and not os.path.exists(path):
            os.mkdir(path)
    return path+file+Ext

class ProcessMystem(protocol.ProcessProtocol):
    def __init__(self,name,tag,data):
        self.data = data.encode("cp1251","ignore")
        self.result = [name,tag,""]
    def connectionMade(self):
        if len(self.data) <= 1000:
            self.transport.write(self.data)
        else:
            buf = ""
            for word in self.data.split(" "):
                buf += " "+word
                if len(buf) > 900:
                    self.transport.write(buf)
                    buf=""
            if len(buf) > 0:
                self.transport.write(buf)
        self.transport.closeStdin()
    def outReceived(self, result):
        self.result[2] += result.replace("\r","").replace("?","").decode('cp1251')
    def processEnded(self, status_object):
        self.deferrer.callback(self.result)

def HTML(key,r):
    print "HTML",r,key
    if "content" not in r.raw[key]: 
        r.raw[key]["error"]=u"отсутствует контент"
        return
    try:
        charset = cp(r.raw[key]["content"])
        if charset["encoding"] is "MacCyrillic": charset["encoding"]="windows-1251"
        ucontent = r.raw[key]["content"].decode(charset["encoding"])
        html = fromstring(ucontent.replace("</html>","").replace("</HTML>","").replace('encoding="'+charset["encoding"]+'"',"")+"</html>")
        r.raw[key]["content"]=ucontent
    except ValueError:
        html = fromstring(r.raw[key]["content"].replace("</html>","").replace("</HTML>","")+"</html>")
    except:
        r.raw[key]["error"]=u"ошибка декодирования html"
        return

    try:
        for tg in ["link","noindex","script","style"]:	map(lambda x: x.drop_tree(),html.xpath("//"+tg))
        r.raw[key]["body"]=clean(html.body.text_content(),r.query["stop"])
        r.raw[key]["plain"]=clean(html.text_content(),r.query["stop"])
        for tg in ["title",["h1","h2","h3","h4","h5","h6"],["b","strong"]]:
            if type(tg) is type(""):
                title=html.xpath("//"+tg)
                tag=tg
            else:
                title=html.xpath("|".join(map(lambda x: "//"+x,tg)))
                tag=tg[0]+"-"+tg[len(tg)-1]
            r.raw[key][tag]=" ".join(map(lambda x: clean(x.text_content(),r.query["stop"]),title)) if title else False
            if r.raw[key][tag] is False: del r.raw[key][tag]


    except:
        r.raw[key]["plain"]=clean(html.text_content(),r.query["stop"])
    del r.raw[key]["content"]


def toHTML(result,r):
    print "toHTML",r
    result,blk=result
    idx=0
    ds=[]
    for key in result:
        d=Deferred()
        if key[0]:
            if key[1].find("<body") >= 0  or key[1].find("<BODY") >=0:
                d.addCallback(HTML,r)
            else:
                d.addCallback(PLAIN,r)
            r.raw[blk[idx]]={"content":key[1].replace("</"," </")}

        else:
            r.raw[blk[idx]]={"error":str(key[1].value)}
        d.callback(blk[idx])
        ds.append(d)
        idx+=1
    return DeferredList(ds)


def PLAIN(key,r):
    print "PLAIN",r,key
    if "content" in r.raw[key]:
        if key is  not "stdin":
            charset = cp(r.raw[key]["content"])
            if charset["encoding"] is "MacCyrillic":  charset["encoding"]="windows-1251"
            r.raw[key]["content"] = r.raw[key]["content"].decode(charset["encoding"])
        r.raw[key]["plain"]=clean(r.raw[key]["content"],r.query["stop"])
        del r.raw[key]["content"]


def add_blk(result,blk):   
    print "add_blk"
    return result,blk


def Parse(result,r):
    result = map(lambda x: x[1],result)
    print r.query["query"]
    parsed = {}
    root2word={}
    word2root={}
    avg=False
    max=False
    min=False
    total=False
    zipf=False
    zipfCount=False
    queries_normal = ("query" in r.query and r.query['query']) and clean(r.query["query"]).split() or False
    queries = ("query_org" in r.query and r.query['query']) and clean(r.query["query_org"]).split() or False


    for entry in result:
        if entry[0] not in parsed:
            parsed[entry[0]]={}
        if entry[1] == "error":
            parsed[entry[0]]["error"]=entry[2]
            continue

        parsed[entry[0]][entry[1]]={"words":{},"count":[]}

        root2word={}
        word2root={}

        t = parsed[entry[0]][entry[1]]
        raw=[]
        wordsLen=0
        wordsCount=0
        zipf=0.
        zipfCount=0
        for word in entry[2].split("\n"):

            re1 = _re1(word)
            if re1 == []: continue
            word,roots = re1[0]
            raw.append(word)
            wordsCount+=1
            wordsLen+=len(word)
            for root in roots.split("|"):
                if root not in root2word: root2word[root]=word
                if word not in word2root: word2root[word]=root
            if root not in t["words"]: 
                t["words"][root]={"procent":0,"count":0,"variant":[],"pos":0}

            t["words"][root]["variant"].append(word)
            t["words"][root]["count"]+=1

        t["count"].append(wordsCount)
        t["count"].append(wordsLen)
        rating=map(lambda x:[t["words"][x]["count"],x],t["words"])
        rating.sort()
        for i in range(len(rating)): 
            t["words"][rating[i][1]]["pos"]=len(rating)-i
        for word in t["words"]:
            t["words"][word]["procent"]=round(float(t["words"][word]["count"])/float(wordsCount)*100,2)
            if entry[1] == "plain":	

                z1=round(float(t["words"][word]["count"])/float(wordsCount),6)
                z2=round(float(float(t["words"][word]["count"])*t["words"][word]["pos"])/float(wordsCount),6)
                t["words"][word]["zipf"]=z2
                zipf+=z2
                zipfCount+=1

        if entry[1] == "plain":
            parsed[entry[0]]["stat"]=[[],[],[],[],[],[],[],[],[]]
            p = parsed[entry[0]]["stat"]

            p[8].append(round(zipf/float(zipfCount),6))
            p[0].append(wordsCount)
            p[1].append(wordsLen)

            if queries:
                for query in queries:
                    found=filter(lambda x: query == x or query == root2word[x],t["words"])

                    if found:
                        normal=reduce(lambda x,y: x+y,map(lambda x: t["words"][x]["count"],found))
                        z=reduce(lambda x,y: x+y ,map(lambda x: t["words"][x]["variant"],found))
                        strong=found and len(filter(lambda x: x == query,z)) or 0
                        form=len(filter(lambda x: x != query,z))
                    else:
                        strong=normal=form=0
                    p[2].append(normal)
                    p[3].append(round(float(normal)/float(t["count"][0])*100,2))
                    p[4].append(strong)
                    p[5].append(round(float(strong)/float(t["count"][0])*100,2))
                    p[6].append(form)
                    p[7].append(round(float(form)/float(t["count"][0])*100,2))
            else:
                for i in [2,3,4,5,6,7]: p[i].append(0)
            if not r.query["yandex"] or (entry[0] in r.text2url['yandex'] and r.text2url['yandex'][entry[0]] in r.query["yandex"]):
                total=merge(total,p)
                min=compare(min,'<',p)
                max=compare(max,'>',p)
    
    if total:
        avg = map(lambda x: type(x) is [int,float] and round(x/len(parsed),6) or map(lambda y: round(y/len(parsed),6),x),total)
        max = map(lambda x:len(x) and x or [0],max)
        min = map(lambda x:len(x) and x or [0],min)
        avg = map(lambda x:len(x) and x or [0],avg)
    for key in parsed:
        if not "stat"  in parsed[key]:         parsed[key]["stat"]=None
        if not parsed[key]["stat"]:            parsed[key]["stat"]=[[0]]*9

    stat={"min":min,"max":max,"avg":avg}
    result = {"result":parsed,"hash":r.hash,'task':r.query,'text2url':r.text2url,'org':r.data,'stat':stat}
    return result

def Save(result,r):
    js = json.dumps(result)
    if r.finished == 0:
        r.write(js)
        r.finish()
    f = open(getPath(r.hash,Make=True),"w")
    f.write(js)
    f.close()
    print "Save",r,len(js),getPath(r.hash,Make=True)
    return True


def Error(e,r):
    print "Exception in user code:"
    print '-'*60
    traceback.print_exc()
    print '-'*60

    if r.finished == 0:
        r.write(json.dumps({"error":str(e.getErrorMessage())}))
        r.finish()

def Fork(result,r):
    print "Fork",r
    if len(result) == 0:
        raise Exception("надо что нибудь вводить")
        return False
    new = {}
    ds = []
    for key in r.raw:
        for tag in r.raw[key]:
            if len(r.raw[key][tag]) > 0 and "error" not in r.raw[key]:
                if tag in ["content"]: continue
                d = Deferred()
                m = ProcessMystem(key,tag,r.raw[key][tag])
                m.deferrer = d
                reactor.spawnProcess(m, config.binary, [config.binary,"-n"], {})
                ds.append(d)

            elif "error" in r.raw[key]:
                d = Deferred()
                d.callback([key,tag,r.raw[key]["error"]])
                ds.append(d)
    return DeferredList(ds)
#mystem = ProcessMystem()


class Mystem(resource.Resource):
    isLeaf = True
    url = re.compile("(http://[^\s]+|https://[^\s]+)")
    plain = re.compile("~([^~]+)~")
    yandex = re.compile("!yandex[\s\t\n\r]?([0-9]+)")
    yandex_region = re.compile("!lr[\s\t\n\r]?([0-9]+)")
    def __init__(self):
        if "binary" not in config:
            print "binary not found in config"
            reactor.stop()
            os.exit()
        global root_path,wait_path,static_path
        root_path = config.static
        static_path = '/static/'+config.path+'/'
        pass
    def render_GET(self,r):

        if  len(r.postpath) == 1 and r.postpath[0].endswith(".js"):
            file = r.postpath[0].replace(".js","")
            r.setHeader('content-type','application/json')
            if os.path.exists(getPath(file)):
                s = os.stat(getPath(file))
                etag = file+"_"+md5(str(s.st_size)+":"+str(s.st_ctime)).hexdigest()
                if r.setETag(etag) is None:
                    r.setHeader('X-Accel-Redirect',static_path + getPath(file,Root=False))
            else:
                r.write("{'try':'%s'}"%file)

        else:
            r.setResponseCode(404)
        r.finish()
        return server.NOT_DONE_YET
    def Complete(self,r):
        if r.finished: 	return
        r.write("{'try':'%s'}"%r.hash)
        r.finish()

    def parseYandex(self,result,r,num):
        r.query["yandex"]=map(lambda x: x.get('href'),fromstring(result).xpath(config.yandex["href"]))[:num]
        text=map(lambda x: x.text_content().strip().lower(),fromstring(result).xpath(config.yandex["text"]))[:num]
        r.text2url["yandex"]=dict(map(lambda x: [text[x],r.query['yandex'][x]],xrange(len(text))))
        print r.text2url
        return DeferredList(map(lambda url: getPage(url),r.query['yandex'])).addCallback(add_blk,text)



    def getYandex(self,text,lr):
        print 'getYandex',config.yandex["pattern"] % { "query":quote(text.encode('utf-8')),"lr":lr and lr or 213 }
        return config.yandex["pattern"] % { "query":quote(text.encode('utf-8')),"lr":lr and lr or 213 }

    def saveQuery(self,result,r):
        r.query["query"] =  " ".join([key[1] for key in  _re1(result[2])])


    def genDeferred(self,task,r):
        ds=[]


        if task["plain"]:
            d = Deferred()
            d.addCallback(add_blk,["stdin"])
            d.addCallback(toHTML,r)
            d.callback([(True,task["plain"])])
            ds.append(d)

        if task["fetch"]:
            ds.append(DeferredList(map(lambda url: getPage(url.encode('utf-8')),task["fetch"])).addCallback(add_blk,task["fetch"]).addCallback(toHTML,r))


        if task["yandex"]:
            d=getPage(self.getYandex(task["query"],task["yandex_region"]))
            d.addCallback(self.parseYandex,r,task["yandex"]).addCallback(toHTML,r)
            d.addErrback(Error,r)
            ds.append(d)

        if task["query"]:
            d = Deferred()
            m = ProcessMystem("task","query",task["query"])
            m.deferrer = d
            reactor.spawnProcess(m, config.binary, [config.binary,"-n"], {})
            d.addCallback(self.saveQuery,r)
            ds.append(d)
        return DeferredList(ds)

    def parseInput(self,r):
        parsed={"yandex":False,"yandex_region":False,"fetch":False,"query":False,"plain":False,"stop":False}
        
        if self.yandex_region.search(r.query):
            parsed["yandex_region"]=int(self.yandex_region.findall(r.query)[0])
            r.query=self.yandex_region.sub("",r.query)
        if self.yandex.search(r.query):
            parsed["yandex"]=int(self.yandex.findall(r.query)[0])
            r.query=self.yandex.sub("",r.query)
            if parsed["yandex"] == 0 or parsed["yandex"] > 10: parsed["yandex"]=False

        if r.query.find("!stop") >= 0:
            parsed["stop"]=True
            r.query=r.query.replace("!stop","")

        if self.plain.search(r.query):
            parsed["plain"]= " ".join(self.plain.findall(r.query))
            r.query=self.plain.sub("",r.query)

        if ((parsed["yandex"] or parsed["plain"]) and self.url.search(r.query) ) or r.query.find("http://") == 0:
            parsed["fetch"]=self.url.findall(r.query)[:10]
            r.query=self.url.sub("",r.query)

        if parsed["yandex"] or parsed["fetch"] or parsed["plain"]:
            parsed["query_org"]=parsed["query"]=r.query.strip()
        elif not parsed["plain"]:
            parsed["plain"]=r.query.strip()

        if parsed["plain"]: r.raw["stdin"]=parsed["plain"]
        r.query = parsed

        return parsed


    def render_POST(self,r):
        print "-"*100
        if "text" not in r.args: 
            r.finish()
            return
        r.setHeader('content-type','application/json')
        r.data  = ("".join(r.args["text"]))
        r.result = {}
        r.raw={}
        r.blk = []
        r.text2url={}
        r.query = r.data.decode('utf-8')
        r.hash = md5(r.data).hexdigest()
        if os.path.exists(getPath(r.hash)): os.unlink(getPath(r.hash))
        r.end = False
        r.d = threads.deferToThread(self.parseInput,r).addCallback(self.genDeferred,r).addCallback(Fork,r).addCallback(Parse,r).addCallback(Save,r).addErrback(Error,r)
        r.l = reactor.callLater(1,self.Complete,r)
        return server.NOT_DONE_YET



