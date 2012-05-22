#!/usr/bin/python
# -*- coding: utf8 -*-


from random import randint
from config import Config
from twisted.python.logfile import DailyLogFile
from twisted.python.log import startLogging,err,failure
import traceback
from twisted.web import server, resource , static
from twisted.internet import threads, reactor, task
from twisted.internet.defer import Deferred,DeferredList
from twisted.web.client import _parse, HTTPClientFactory
from md5 import md5

import re
import sys
import signal
import re
import pty
import termios
import os
import logging
from time import time
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

from pprint import pprint
from datetime import date as datetime
from twisted.internet import protocol
import copy

from lxml import etree

def printError(self, *arg):
    import pdb
    pdb.set_trace()
    print 'ERROR',arg
    print '-='*100
    print self.value

IP = [ 
    '213.248.54.138', '213.248.58.154', '213.248.58.127', '213.248.58.128',
    '213.248.58.126', '213.248.58.151', '213.248.58.152', '213.248.58.153',
    '213.248.58.155', '213.248.58.130', '213.248.58.129'
]

def _getPage(url, contextFactory=None, *args, **kwargs):
    scheme, host, port, path = _parse(url)
    factory = HTTPClientFactory(url, *args, **kwargs)
    ip = IP[randint(0,len(IP)-1)]
    reactor.connectTCP(host, port, factory, bindAddress = (ip,0))
    return factory.deferred
                                                                                    

def getPage(url, timeout = 60): return _getPage(url,timeout=timeout,agent="Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; API.ADVPEOPLE.RU)" )

class NotFound(object):
    message = "not found"

class Poller(list):
    def __init__(self):
        task.LoopingCall(self.process).start(1)
    def repeat(self, err, proc):
        try:
            code = err.value[0]
        except:
            code = None
        if  code == '410':
            print 'ERROR POLLER',err, proc[0]
            proc[2].errback(NotFound())
        elif  code == '404':
            proc[2].errback(NotFound())
        elif  code == '200':
            self.append(proc)
        else:
            print 'ERROR POLLER',err, proc[0]
            proc[3] = proc[3] - 1
            self.append(proc)

    def process(self):
        i = len(self)
        if i == 0:
            return
        if config.debug:
            print 'Poller -> process', procs
        for i in xrange(config.limit * len(IP)):
            if len(self) == 0:
                break
            pos = randint(0,len(self)-1)

            proc = self[pos]
            self.remove(proc)
            url, cb, defered, num = proc
            if num:
                getPage(url).addCallback(cb, defered).addErrback(self.repeat, [url, cb, defered, num ])
            else:
                print 'ERRRRROR :( ', url, num

QUEUE = Poller()
STATUS = {}


def getFileInfo(filename):
    try:
        return os.stat(filename)
    except:
        return False
def getFileMtime(filename):
    try:
        st = os.stat(filename)
        return st.st_ctime
    except:
        return False

def uniqueList(List):
    outList = []
    for v in List:
        if v not in outList:   
            outList.append(v)
    del List
    return outList
    

def makePath(user):
    return config.static+("/".join(list(user)[:4]))+'/'+user



class Crawler(object):
    def __init__(self, user, defered, Depth = 1):
        self.finish_defered = defered
        self.user = user
        self.hash = md5(str(user)+'nobodyknow').hexdigest()
        self.depth = Depth
        self.time = time()
        self.total_friends = 0
        self.done_friends = 0
        if Depth > 0:
            STATUS[self.hash] = 0
        if config.debug:
            print self,'http://%s.livejournal.com/data/foaf'%(user)
        defered1 = Deferred()
        defered2 = Deferred()

        if self.user[0] in ("_","-") or self.user[-1] in ("-", "_"):
            QUEUE.append([str('http://users.livejournal.com/%s/data/foaf'%(self.user)), self.parseFoafResult, defered1, 10])
        else:
            QUEUE.append([str('http://%s.livejournal.com/data/foaf'%(self.user.replace("_","-"))), self.parseFoafResult, defered1, 10])
        QUEUE.append([str('http://www.livejournal.com/misc/fdata.bml?comm=1&user=%s'%(self.user)), self.parseFriendListResult, defered2, 10])
        DeferredList([defered1, defered2]).addCallback(self.repackResult).addCallback(self.fetchFriendProfile).addCallback(self.returnResult).addErrback(self.err)
    
    def err(self, err):
        print self, 'ERROR' ,err, err.getTraceback()

        self.finish_defered.errback(err)

    def parseFriendListHTMLResult(self, data, defered_at_end):
        if config.debug:
            print self, 'parseFriendListHTMLResult', defered_at_end, self.user
        root = fromstring(data)
        fofs = map(lambda el:el.text, root.xpath('//div[@id="fofs_body"]/a'))
        mofs = map(lambda el:el.text, root.xpath('//div[@id="mofs_body"]/a'))
        if len(fofs) == 0:
            fofs = map(lambda el:el.text, root.xpath('//div[@id="friends_body"]/a'))
            mofs = map(lambda el:el.text, root.xpath('//div[@id="watching_body"]/a'))

        defered_at_end.callback({'friends_of':fofs,'community':mofs})

    def parseFriendListResult(self, data, defered_at_end):
        if config.debug:
            print self, 'parseFriendListResult', defered_at_end, self.user
        try:
            data = map(lambda x: x.split(), filter(lambda x: len(x) >0 and x[0] in ('P', 'C'),data.split('\n')))
        except:
            print 'ERROR',data
        if len(data) > 2499:
            if self.user[0] in ("_","-") or self.user[-1] in ("-", "_"):
                QUEUE.append([str('http://users.livejournal.com/%s/profile?mode=full'%(self.user)), self.parseFriendListHTMLResult, defered_at_end, 10])
            else:
                QUEUE.append([str('http://%s.livejournal.com/profile?mode=full'%(self.user.replace("_","-"))), self.parseFriendListHTMLResult, defered_at_end, 10])
        else:
            friendsOf = []
            community = []
            for flag,nick in data:
                if flag == 'P<':
                    friendsOf.append(nick)
                elif flag[0] == 'C':
                    community.append(nick)

            defered_at_end.callback({'friends_of': uniqueList(friendsOf), 'community': uniqueList(community)})


    def parseFoafResult(self, data, defered_at_end):
        if config.debug:
            print self, 'parseFoafResult',  defered_at_end, self.user
        try:
            nick = re.findall("""<foaf:nick>([^<]+)</foaf:nick>""", data)
        except:
            print 'ERRROR',self.user

        try:
            posted = re.findall("""<ya:posted>([0-9]+)</ya:posted>""", data)[0]
        except:
            posted = 0
        try:
            city = re.findall("""<ya:city dc:title="([^"]+)""", data)[0]
            city = urlparse.unquote(city)
        except:
            city = ""
        try:
            country = re.findall("""<ya:country dc:title="([^"]+)""", data)[0]
        except:
            country = ""

        try:
            bio = re.findall("""<ya:bio>([^<]+)</ya:bio>""", data)[0]
        except:
            bio = ""
        try:
            interest = re.findall("""<foaf:interest dc:title="([^"]+)""", data)
        except:
            interest = []

        try:
            name = re.findall("""<foaf:name>([^<]+)</foaf:name>""", data)[0]
        except:
            name = self.user
        try:
            date = re.findall("""<foaf:dateOfBirth>([^<]+)</foaf:dateOfBirth>""", data)[0]
        except:
            date = ''

        friends = nick[1:]
        nick = nick[0]
        
        result = {
            'nick': nick,  'interest': interest, 'name': name, 
            'country': country, 'city' : city, 
            'date' : date,
            'num': len(friends), 'posts': posted,
            'friends': dict(map(lambda friend: [friend,None],friends)),
        }
        defered_at_end.callback(result)

    def repackResult(self, result):
        if config.debug:
            print self, 'repackFriendProfile', self.user, self.depth
        (foaf_flag, foaf),(friends_flag, friends) = result
        if not foaf_flag:
    	    return foaf

        if not friends_flag:
    	    return  friends

        foaf['friends_of'] = dict(map(lambda entry: [entry, None], friends['friends_of']))
        foaf['community'] = dict(map(lambda entry: [entry, None], friends['community']))
        return foaf

    def collectStats(self, result):
        self.done_friends+=1
        if self.depth:
            STATUS[self.hash] =  100.0/float(self.total_friends)*float(self.done_friends)
        return result
    def fetchFriendProfile(self, foaf):
    
        if config.debug:
            print self, 'fetchFriendProfile', self.user, self.depth
        if 'friends_of' not in foaf:
            return foaf
        if self.depth > 0:
            users = uniqueList(foaf['friends_of'].keys()+foaf['friends'].keys()+foaf['community'].keys())
            self.total_friends = len(users)
            defereds = []
            for user in users:
                if len(filter(lambda x:x,map(lambda x:user.endswith(x), ['.ru','.com']))) > 0:
                    continue
                if user.find(' ') >= 0:
                    continue
                if user == self.user:
                    continue
                defered = Deferred().addCallback(self.collectStats)
                Crawler(user, defered, self.depth - 1)
                defereds.append(defered)
            return DeferredList(defereds).addCallback(self.mergeResult, foaf)
        return foaf

    def mergeResult(self, childs, root):
        if config.debug:
            print self, 'mergeResult', self.finish_defered, self.user
        for (result, child) in childs:
            if result:
                nick = child['nick']

                if nick in root['friends']:
                    root['friends'][nick] = child 

                if nick in root['friends_of']:
                    root['friends_of'][nick] = child 

                if nick in root['community']:
                    root['community'][nick] = child 
        return root

    def returnResult(self, result):
        if config.debug:
            print self,'returnResult', self.finish_defered, self.user
        self.finish_defered.callback(result)


class LJ(resource.Resource):
    isLeaf = True
    def __init__(self):

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
                r.write(json.dumps({'try':file}))
        else:
            r.setResponseCode(404)
        r.finish()
        return server.NOT_DONE_YET



    def render_POST(self,r):
        print "-"*100
        print r.args
        if "ljuser" not in r.args: 
            r.setResponseCode(500)
            r.finish()
            return server.NOT_DONE_YET 
        r.setHeader('content-type','application/json')

        def crawler_finish(res, hash):
            interest = {}
            groups = {}
            dates = {}

            #dates
            date_total = 0
            cur_year = datetime.today().year

            for users in [res['friends'], res['friends_of'], res['community']]:
                for key in users:
                    if key == res['nick']:
                        user=res
                    else:    
                        user = users[key]
                    if not user:    
                        continue
                    try:
                        date = user['date'].split('-')[0]
                        date = int(date)
                    except:
                        date = 0

                    if date < 1900:
                        date = 'не указали'
                    else:    
                        date = cur_year - date
                        if date <= 12:
                            date = '0-12'
                        elif date > 12 and date < 15:
                            date = '12-15'
                        elif date > 15 and date < 18:
                            date = '15-18'
                        elif date > 18 and date < 21:
                            date = '18-21'
                        elif date > 21 and date < 25:
                            date = '21-25'
                        elif date > 25 and date < 30:
                            date = '25-30'
                        elif date > 30 and date < 40:
                            date = '30-40'
                        else:
                            date = '40-&#8734;'
                    if  date not in dates:
                        dates[date]={'users':[], 'count': 0}
                    if key not in dates[date]['users']:
                        dates[date]['users'].append(key)
                    dates[date]['count']+=1
                    date_total +=1
            for key in dates:
                user = ", ".join(dates[key]['users'][:8])
                if len(dates[key]['users']) > 8:
                    user+='... и ещё %d пользователей' % (len(dates[key]['users'])-8 )

                dates[key]['users'] = user    
                dates[key]['count'] =  '%.2f' % (100.0/float(date_total)*float(dates[key]['count']))

            to_sort = []
            for key in dates:
                to_sort.append((key, dates[key]['users'], dates[key]['count']))
            dates = sorted(to_sort, key = lambda x: x[2], reverse = True)[:40]

            #interes
            for users in [res['friends'], res['friends_of'], res['community']]:
                for key in users:
                    if key == res['nick']:
                        user=res
                    else:    
                        user = users[key]
                    if not user or not user['interest']:    
                        continue
                    for interes in user['interest']:

                        if interes not in interest:
                            interest[interes]={'users':[], 'count': 0}
                        if key not in interest[interes]['users']:
                            interest[interes]['users'].append(key)
                        interest[interes]['count']+=1
            for key in interest:
                user = ", ".join(interest[key]['users'][:8])
                if len(interest[key]['users']) > 8:
                    user+='... и ещё %d пользователей' % (len(interest[key]['users'])-8 )
                interest[key]['users'] = user    

            to_sort = []
            for key in interest:
                to_sort.append((key, interest[key]['users'], interest[key]['count']))
            interest = sorted(to_sort, key = lambda x: x[2], reverse = True)[:40]

            #groups    
            for users in [res['friends'], res['friends_of']]:
                for key in users:
                    if key == res['nick']:
                        user=res
                    else:    
                        user = users[key]
                    if not user or not user['community']:    
                        continue
                    for group in user['community']:
                        if group not in groups:
                            groups[group]={'users':[], 'count': 0}
                        if key not in groups[group]['users']:
                            groups[group]['users'].append(key)
                        groups[group]['count']+=1
            for key in groups:
                user = ", ".join(groups[key]['users'][:8])
                if len(groups[key]['users']) > 8:
                    user+='... и ещё %d подписчиков' % (len(groups[key]['users'])-8 )
                groups[key]['users'] = user    

            to_sort = []
            for key in groups:
                to_sort.append((key, groups[key]['users'], groups[key]['count']))
            groups = sorted(to_sort, key = lambda x: x[2], reverse = True)[:40]



            res = {'groups': groups, 'interest': interest, 'dates': dates}    

            if getFileInfo(os.path.dirname(makePath(hash))) is False:
                os.makedirs(os.path.dirname(makePath(hash)))
            open(makePath(hash),'w').write(json.dumps(res))

            if hash in STATUS:
                del STATUS[hash]



        def crawler_error(res, hash):
            print '!'*30
            print 'CRAWLer ERROR', res
            STATUS[hash] = -1


        user  = str(("".join(r.args["ljuser"])).decode('utf-8'))
        hash = md5(user+'nobodyknow').hexdigest()


        if getFileMtime(makePath(hash)) + 5*86400 > time():
            r.setHeader('X-Accel-Redirect', config.cache + makePath(hash).replace(config.static,''))

        elif hash in STATUS:
            print len(map(lambda ent: ent[0], QUEUE))
            r.write(json.dumps({'processed': '%.2f'%(STATUS[hash])}))

        else:
            r.write(json.dumps({'try':hash}))

            threads.deferToThread(lambda:  Crawler(user, Deferred().addCallback(crawler_finish, hash).addErrback(crawler_error, hash)))

        r.finish()
        return server.NOT_DONE_YET


