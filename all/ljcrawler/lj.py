#!/usr/bin/python
# -*- coding: utf8 -*-


from config import Config
try:
    from twisted.internet import epollreactor
    epollreactor.install()
except:
    pass
from twisted.python.logfile import DailyLogFile
from twisted.python.log import startLogging,err
import traceback
from twisted.web import server, resource , static
from twisted.application import internet, service
from twisted.internet import threads,reactor
from twisted.internet.defer import Deferred
from twisted.web.client import getPage
from md5 import md5
import re
import sys
import signal
import os
import pty
import logging
import time
import Queue
import StringIO
import urlparse

from optparse import OptionParser


sys.path.insert(0,'./')

class Page(resource.Resource):
    isLeaf=True
    def render(self, request):
        print request
        return "Page\n"

class Site(resource.Resource):
    def __init__(self,config):
        resource.Resource.__init__(self)
        for action in config.actions:
            name = action.name
            action.debug = config.debug
            action.static = config.static+'/'+action.path+'/'
            action.cache = '/cache/'+action.path+'/'
            module = __import__("action"+name,globals(), locals(), [name])
            setattr(module,"config",action)
            self.putChild(action.path,getattr(module,name)())
    def render(self, request):
        print request
        return "ddd\n"


def signalhandler(*args):
    reactor.stop()
    sys.exit()
signal.signal(signal.SIGUSR1, signalhandler)
signal.signal(signal.SIGHUP, signalhandler)

def error(msg):
    print '\n\t\t%s\n\t\t%s sample.cfg\n' % (msg,sys.argv[0])
    sys.exit()
try:
    f = file(sys.argv[1] if 1 in sys.argv else 'config.json')
except IOError:
    error("файл не найдет")
config = Config(f)

try:
    fp = open(config.pid,'r')
    pid = fp.read()
    fp.close()
    os.kill(int(pid), signal.SIGUSR1)
    time.sleep(0.1)
    os.kill(int(pid), signal.SIGINT)
except:
    pass
os.environ['PYTHON_EGG_CACHE'] = '/lj/'
if "debug" not in config: 
    pid = os.fork()
    if  pid  == 0:
        class NullDevice:
            def write(self, s):
                f = open("/tmp/tools.log","a")
                f.write(str(s))
                f.close()
                pass
        sys.stdin.close()
        os.setsid() 
        sys.stdout = NullDevice()
        sys.stderr = NullDevice()
        os.chdir("/") 
    else:
        fp = open(config.pid,'w')
        fp.write(str(pid))
        fp.close()
        print "Done"
        sys.exit()
os.umask(0)
if "uid" in config:
    try:
        os.setgid(config.gid)
    except OSError:
        print 'running not from root, skiping change uid' 
if "gid" in config:
    try:
        os.setuid(config.uid)
    except:
        pass
srv = service.MultiService()

try:
    site = server.Site(Site(config))
    #reactor.listenTCP(config.port, site)
    internet.TCPServer(config.port, site).setServiceParent(srv)
except:
    print "Exception in user code:"
    print '-'*60
    traceback.print_exc(file=sys.stdout)
    print '-'*60
    error("не понятная ошибка при запуске")
application = service.Application("LJ")
srv.setServiceParent(service.IServiceCollection(application))
if __name__ == '__main__':
    reactor.run()
