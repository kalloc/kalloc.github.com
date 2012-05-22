#!/usr/bin/python
# -*- coding: utf8 -*-

from config import Config
import sys,os,xmpp,time,platform,time,random, traceback, signal
from md5 import md5
from twisted.internet import epollreactor
epollreactor.install()
from twisted.internet import reactor,protocol
from twisted.internet.task import LoopingCall
from twisted.internet.protocol import Protocol, ClientFactory, Factory
from twisted.words.protocols.jabber import client, jid  as JID ,component
from twisted.words.xish import domish, xmlstream
from twisted.python.logfile import DailyLogFile
from twisted.python.log import startLogging,err
    
from twisted.web.client import getPage


version="0.03"

def numbericToString(ip):
	return '%d.%d.%d.%d' % (ip & 255,ip>>8 & 255,ip>>16 & 255,ip>>24 & 255) 

def toLow(Config):
	config = {}
	for key in Config:
		config[key.lower()]=Config[key]
	return config


class NetString(object):
	field=[]
	notifies={}
	def __init__(self,string,replace):
		length,fieldNum,string,stringNum=self.parse(string)
		for i in range(int(fieldNum)):
			length,value,string,stringNum=self.parse(string)
			self.field.append(value)
		while string:
			notify={}
			for i in range(int(fieldNum)):
			
				length,value,string,stringNum=self.parse(string)
				if string == "": break
				notify[self.field[i]]=replace[self.field[i]] if self.field[i] in replace else value
			if len(notify) > 0:
				if notify["Recipient"] not in self.notifies:
					self.notifies[notify["Recipient"]]=[]
				self.notifies[notify["Recipient"]].append(notify)

	def parse(self,s):
		pos=s.find(":")
		length=s[:pos]
		if length[0] == 'D':
			return [0,"","",s[pos+1:]]
		length=int(length)
		return [length,s[pos+1:length+pos+1],s[length+pos+2:],0]
		
	
	def getNotify(self):
		if len(self.notifies) == 0:
			return False
		key = self.notifies.keys()[0]
		return [key,self.notifies.pop(key)]




class Source(object):
	def __init__(self,inf,source):
		self.config = toLow(source)
		self.informer = inf
		self.config["replace"] = self.config["replace"] if "replace" in self.config else {}
		self.config["send"] = self.config["send"].replace('%InformerId%',inf.id)
		
		reactor.callLater(1,LoopingCall(getList,self).start,self.config["period"])
		
	def callback(self,result):
		"""
		
2:16,4:DBid,9:Recipient,6:Status,8:HostName,8:LObjName,8:ModuleId,8:MethodId,4:Port,8:LastDown,6:LastUp,8:Template,2:IP,4:Host,7:Gateway,6:Sender,13:AddParameters,

5:54299,22:ivan@solomonitoring.ru,1:1,7:radabor,9:!!!!!!!!1,1:3,1:4,2:80,10:1268535593,10:1270840082,399:#hostname# - имя хоста, на котором произошло событие
#host# - хост, на котором произошло событие
#object# - имя объекта мониторинга с которым произошло событие
#port# - порт
#status# - статус объекта мониторинга
#duration# -
#datetime# - время события,10:1432927321,14:www.radabor.ru,0:,22:info@solomonitoring.ru,0:,5:54304,16:yuri@timofeev.su,1:0,12:metro2033.ru,29:http://metro2033.ru/index.php,1:3,0:,2:80,10:1270840176,10:1270840144,74:#datetime#,
Host: #host#,
Object: #object#,
Port:#port#,
Status: #status#.,9:354916414,12:metro2033.ru,0:,22:info@solomonitoring.ru,0:,5:54305,20:t.vorobyov@gmail.com,1:0,12:metro2033.ru,29:http://metro2033.ru/index.php,1:3,0:,2:80,10:1270840176,10:1270840144,166:#datetime#
Имя хоста: #hostname#
Хост: #host#
Объект мониторинга: #object#
Порт: #port#

Статус сменился на: #status#,9:354916414,12:metro2033.ru,0:,22:info@solomonitoring.ru,0:,5:54306,14:anton@atlex.ru,1:0,12:metro2033.ru,29:http://metro2033.ru/index.php,1:3,0:,2:80,10:1270840176,10:1270840144,166:#datetime#
Имя хоста: #hostname#
Хост: #host#
Объект мониторинга: #object#
Порт: #port#

Статус сменился на: #status#,9:354916414,12:metro2033.ru,0:,22:info@solomonitoring.ru,0:,5:54307,14:anton@atlex.ru,1:0,12:metro2033.ru,29:http://metro2033.ru/index.php,1:3,0:,2:80,10:1270840176,10:1270840144,74:#datetime#,
Host: #host#,
Object: #object#,
Port:#port#,
Status: #status#.,9:354916414,12:metro2033.ru,0:,22:info@solomonitoring.ru,0:,D1:5	
		"""
		if self.informer.availableJabber():
			print result
			self.trySend(NetString(result,self.config["replace"]))
	
	def trySend(self,ns,repeat=0):
		notify = ns.getNotify()
		if notify is False: return
		if "append" in self.config:
			notify[0] = notify[0]+self.config["append"]
		print notify[0]
		jabber = self.informer.getJabber(notify[0])

		if jabber == False:
			jabber  = self.informer.getRandomJabber()
			jabber.Subscribe(notify[0])
			reactor.callLater(3,self.send,jabber,notify)
		else:
			self.informer.setStatus(notify[0])
			if filter(lambda x: x["AddParameters"] == "ReInitUIN",notify[1]):
				jabber.Unsubscribe(notify[0])
				reactor.callLater(3,self.send,jabber,notify)
			else:
				self.send(jabber,notify)
		reactor.callLater(0.1,self.trySend,ns,0 if jabber else repeat + 1)

			
	def send(self,jabber,notify):
		for n in notify[1]:
			dura = int(n["LastUp"]) - int(n["LastDown"])
			dura = "%02d:%02d"%(dura/3600,(dura%3600)/60)
			n["Status"]=int(n['Status'])
			t = time.gmtime(n["Status"] and  float(n["LastUp"]) or float(n["LastDown"]))
			t = '%04d.%02d.%02d %02d:%02d:%02d' % (t[0],t[1],t[2],t[3],t[4],t[5])
			msg = n["Template"].replace("#method#",n["MethodId"]).replace("#datetime#",t).replace("#module#",n["ModuleId"]).replace("#hostname#",n["HostName"]).replace("#ip#",numbericToString(int(n["IP"]))).replace("#port#",n["Port"]).replace("#duration#",n["Status"] and dura or "").replace("#object#",n["LObjName"]).replace("#status#",n["Status"] and "UP" or "DOWN").replace("#host#",n["Host"])
			jabber.sendMessage(notify[0],msg,n)


class Report(object):
	class InformerClientFactory(ClientFactory):
		class InformerProtocol(Protocol):
			def __init__(self,report):
				self.report = report
				self.report._send = False
			
			def connectionMade(self):
				self.report._send = self.transport.write
			
			def connectionLost(self,reason):
				self.report.process = False
				self.report._send = False
			
		def __init__(self,report):
			self.report = report
		
		def buildProtocol(self, addr):
			return self.InformerProtocol(self.report)
		
	def connect(self):
		print "Report connect"
		self.process  = True
		print self.config["host"], self.config["port"]
		reactor.connectTCP(self.config["host"], self.config["port"],self.InformerClientFactory(self))
		
	def __init__(self,inf,report):
		self.informer = inf
		self.id = inf.id
		self._send = False
		self.process = False
		self.config = toLow(report)
		
		self.errorURL = self.config["errorurl"].replace("%InformerId%",self.informer.id)
		self.okURL = self.config["doneurl"].replace("%InformerId%",self.informer.id)
	
	def send(self,URL):
		if self.process == False:
			print "Report send CONNECT"
			self.connect()
			reactor.callLater(0.5,self.send,URL)
		elif self._send == False:
			print "Report send WAIT"
			reactor.callLater(0.2,self.send,URL)
		else:
			print "Report send %s"%URL
			self._send(URL)

	def sendReport(self,n):
		if self.informer.getStatus(n["Recipient"]):
			URL = self.okURL.replace('%DBid%',n["DBid"])
		else:
			URL = self.errorURL.replace('%DBid%',n["DBid"])
		self.send(URL)
		


class Jabber(object):
	def _addContact(self,jabber,contact):
		if contact not in self._jabber[jabber.getJID()].roster:
			self._jabber[jabber.getJID()].roster.append(contact)
		if contact not in self._roster:
			self._roster[contact]=jabber

	
	def getStatus(self,jid):
		return self._status[jid] if jid in self._status else True

	def checkStatus(self,jid):
		return True if jid in self._status else False
	
	def setStatus(self,jid,Status=True):
		self._status[jid]=Status
	
	def getJabber(self,contact):
		return self._roster[contact] if contact in self._roster else  False

	def _delContact(self,jabber,contact):
		if contact in self._jabber[jabber.getJID()].roster:
			self._jabber[jabber.getJID()].roster.remove(contact)
		if contact in self._roster:
			del self._roster[contact]
		if contact in jabber.roster:
			jabber.roster.remove(contact)
		if contact not in self._remove:
			self._remove.append(contact)

		
	
	def getRoster(self,jid):
		return self._jabber[jid] if jid in self._jabber.roster else False		

	def _addClient(self,jabber):
		self._jabber[jabber.config["jid"]]=jabber
		jabber.roster = []

	def _delClient(self,jabber):
		if jabber.config["jid"] in self._jabber:
			self._jabber.remove(jabber.config["jid"])
		for key in self._roster.keys():
			if self._roster[key] == jabber:
				_roster.remove(key)
			if key in _status:
				self._status.remove(key)


class JabberClient(object):
	def __init__(self,inf,jabber):

		self.config=toLow(jabber)
		self.informer = inf
		self.active = False

		Jabber.__init__(self)
		
		factory = client.basicClientFactory(JID.JID(self.getJID()+'/'+md5(platform.node()).hexdigest()), self.config["password"])
		factory.addBootstrap("//event/stream/authd", self.onConnect)
		factory.addBootstrap("/iq/error", self.onAuthfail)
		reactor.connectTCP(self.config["host"],5222,factory)
		self.factory = factory

	def delContact(self,contact):
		print self.informer._delContact(self,contact)


	
	def getJID(self):
		return self.config["jid"]
	def getHost(self):
		return self.config["host"]
	def onAuthfail(self,iq):
		print u"JID IGNORED — «%s». Ошибка — «%s», код — «%s».\n"%(self.getJID(),iq.error["type"],iq.error["code"])
	def onError(self,message):
		message.handled=True
		if message.error["code"] == "500":
			reactor.callLater(1800,JabberClient,self.config)
			self._delClient(self)
			self.xs.send("</stream:stream>")
			self.xs.iqDeferreds={}
			self.factory.stopTrying()
			self.xs.transport.loseConnection()
			
		else:
			self.informer.setStatus(message["from"],False)

	def onConnect(self,xmlstream):
		print "auth"
		self.active = True

		self.xs=xmlstream
		self.informer._addClient(self)
		presence = domish.Element(('jabber:client','presence'))
		presence.addElement('priority').addContent("100")

		presence.addElement("show", None, ":)")
		presence.addElement('status').addContent('Online')
		xmlstream.send(presence)
		xmlstream.addObserver('/iq',       self.onIq) 
		xmlstream.addObserver('/message',  self.onMessage)
		xmlstream.addObserver('/presence', self.onPresence)

		xmlstream.addObserver('/presence[@type="unsubscribe"]', self.onPresenceUnsubscribe)
		xmlstream.addObserver('/presence[@type="unsubscribed"]', self.onPresenceUnsubscribed)
		xmlstream.addObserver('/presence[@type="subscribe"]', self.onPresenceSubscribe)
		xmlstream.addObserver('/iq/query[@xmlns="jabber:iq:roster"]',self.onRoster)
		xmlstream.addObserver('/iq/query[@xmlns="jabber:iq:version"]',self.onVersion)
		xmlstream.addObserver('/iq/time[@xmlns="urn:xmpp:time"]', self.onTime) 
		xmlstream.addObserver('/iq/query/item[@subscription="remove"]', self.onRemove) 
		xmlstream.addObserver('/message[@type="chat"]/body', self.onChat) 
		xmlstream.addObserver('/message[@type="error"]/error', self.onError) 
		
		iq = component.xmlstream.IQ(xmlstream, 'get')
		iq['type'] = 'get'
		query = iq.addElement('query')
		query['xmlns']='jabber:iq:roster'
		iq.send()
		
		
		""" keep-alive """
		lc = LoopingCall(xmlstream.send, ' ')
		xmlstream.addObserver('//event/stream/end', lambda _: lc.stop())
		lc.start(60)

	def onPresence(self,presence):
		print 'presence'
		print presence.toXml().encode('utf-8')
		print "="*20
		self.iqResult(presence)
		

	def onRoster(self,iq):
		iq.handled=True
		self.iqResult(iq)
		for item in iq.query.elements(): 
			self.addContact(item["jid"])


	def onPresenceUnsubscribed(self,presence):
		presence.handled = True
		self.iqResult(presence)
		
		print "Unsubscribed %s" % presence["from"]

		if presence["from"] in self.informer._roster:

			jid = presence["from"]

			self.Unsubscribe(jid)
			self.delContact(jid)
			presence = domish.Element(('jabber:client','presence'))
			presence["to"] = jid
			presence["type"] = "unsubscribe"
			self.xs.send(presence)
			presence["type"] = "unsubscribed"
			self.xs.send(presence)



	def onPresenceUnsubscribe(self,presence):
		print "Unsubscribe %s" % presence["from"]
		presence.handled = True
		self.iqResult(presence)

		if presence["from"] in self.informer._roster:
			jid = presence["from"]
			self.Unsubscribe(jid)
			self.delContact(jid)
			print "Unsubscribe %s (send unsubscribe)" % jid
			presence = domish.Element(('jabber:client','presence'))
			presence["to"] = jid
			presence["type"] = "unsubscribe"
			self.xs.send(presence)
			presence["type"] = "unsubscribed"
			self.xs.send(presence)

	def iqResult(self,inIq):
		if not inIq.hasAttribute("id"):
			return
		
		iq = domish.Element((None,'iq'))
		iq["type"]= "result"
		iq["id"] = inIq["id"]
		iq["to"] = self.getJID()
		self.xs.send(iq)

	def onRemove(self,iq):
		iq.handled = True
		self.delContact(iq.query.item["jid"])
		print "Remove %s" % iq.query.item["jid"]
		self.iqResult(iq)



	def onPresenceSubscribe(self,presence):
		presence.handled = True
		presence.swapAttributeValues("to", "from")
		self.informer.setStatus(presence["to"])

		presence["type"] = "subscribed"
		self.xs.send(presence)
		self.addContact(presence["to"])


	def onPresenceSubscribed(self,presence):
		presence.handled = True
		presence.swapAttributeValues("to", "from")
		self.infromer.setStatus(presence["to"])
		presence["type"] = "subscribe"
		self.xs.send(presence)
		self.addContact(presence["to"])


	def addContact(self,contact):

		self.informer._addContact(self,contact)

	def Unsubscribe(self,jid):


		print "Unsubscribe %s (remove from roster)"  % jid
		iq = domish.Element((None,'iq'))
		iq["type"]="set"
		item = iq.addElement(('jabber:iq:roster','query')).addElement('item')
		item["jid"] = jid
		item["subscription"] = "remove"
		self.xs.send(iq)


		print "Unsubscribe end"


	def Subscribe(self,jid):
		print "Subscribe %s (add to roster)" % jid
		iq = domish.Element((None,'iq'))
		iq["type"]="set"
		iq["id"] = "abc1a"
		item = iq.addElement(('jabber:iq:roster','query')).addElement('item')
		item["jid"] = jid
		item["name"] = jid
		self.xs.send(iq)


		print "Subscribe %s (send subscribe)" % jid

		presence = domish.Element(('jabber:client','presence'))
		presence["to"] = jid
		presence["type"] = "subscribe"
		presence.addElement(('http://jabber.org/protocol/nick','nick')).addContent("Solomonitoring.RU")
		presence.addElement(('http://jabber.org/protocol/caps','c')).addElement('status').addContent("SoloMonitoring.RU")
		self.xs.send(presence)
		print "Subscribe %s (end)" % jid

	def sendMessage(self,to,msg,n):
		stanza = domish.Element((None,'message'))
		stanza['type'] = 'chat'
		stanza['to'] = to
		stanza['from'] = self.getJID()
		stanza.addElement("body").addContent(msg.decode('utf-8'))
		self.xs.send(stanza)
		reactor.callLater(0.5,self.informer.getRandomReport().sendReport,n)
		
	def onIq(self,iq):
		print 'iq'
		print iq.toXml().encode('utf-8')
		print "="*20
		self.iqResult(iq)

	def onVersion(self,iq):
		iq.handled = True
		iq.swapAttributeValues("to", "from")
		iq["type"] = "result"
		iq.query.addElement("name", None, 'Informer')
		iq.query.addElement("version", None, version)
		iq.query.addElement("os",None,platform.system()+' - '+platform.release())
		self.xs.send(iq)


	def onChat(self,message):
		stanza = domish.Element((None,'message'))
		stanza['type'] = 'chat'
		stanza['to'] = message["from"]
		stanza['from'] = self.getJID()
		print message.body.toXml()
		stanza.addElement("body").addContent(u"Вся информация на сайте solomonitoring.ru")
		self.xs.send(stanza)


	def onTime(self,iq):
		iq.handled = True
		iq.swapAttributeValues("to", "from")
		iq["type"] = "result"
		iq.time.addElement("tzo", None, '%s%02d:00'% (time.timezone > 0 and "" or "-",abs(int(time.timezone)/3600)))
		iq.time.addElement("utc", None, time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()))
		self.xs.send(iq)


	def onMessage(self,elem):
	
		print 'message'
		print elem.toXml().encode('utf-8')
		print "="*20


		
		
class getList(object):
	class InformerClientFactory(ClientFactory):
		class InformerProtocol(Protocol):
			def __init__(self,source):
				self.send = source.config["send"]
				self.callback = source.callback
				self.buffer = ""
	
			def dataReceived(self, data):
				self.buffer+=data
			def connectionMade(self):
				self.transport.write(self.send)
	
			def connectionLost(self,reason):
				self.callback(self.buffer)
	
		def __init__(self,source):
			self.config = source
		def buildProtocol(self, addr):
			return self.InformerProtocol(self.config)
	def __init__(self,source):
		for jid in source.informer._remove:
			if jid in source.informer._roster:
				del source.informer._roster[jid]
				source.informer._remove.remove(jid)
		reactor.connectTCP(source.config["host"], source.config["port"],self.InformerClientFactory(source))

class Informer(Jabber):
	_report=[]
	_jabberInst=[]
	_jabber={}
	_source=[]
	_roster={}
	_status={}
	_remove=[]
	_rreport=[]
	_rjabber=[]


	def __init__(self,informer):
		self.id = str(informer.id)
		for jabber in informer.jabber:
			if "Disabled" not in jabber:
				Informer.addJabber(self,jabber)
		for source in informer.source:
			Informer.addSource(self,source)
		for report in informer.report:
			Informer.addReport(self,report)

	def addJabber(self,jabber):
		j = JabberClient(self,jabber)
		Informer._jabberInst.append(j)
		Informer._jabber[j.getJID()]=j


	def addSource(self,source):
		s = Source(self,source)
		Informer._source.append(s)


	def addReport(self,report):
		r = Report(self,report)
		Informer._report.append(r)
		
	def getRandomJabber(self):
		if not self.availableJabber(): 
			return False
		if len(self._rjabber) == 0:
			self._rjabber = self._jabber.keys()

		key = random.randrange(0,len(self._rjabber)) if len(self._rjabber) > 1 else 0
		print "self._rjabber",self._rjabber
		print "getRandomJabber jids[%d] = %s" % (key,self._rjabber[key])
		return Informer._jabber[self._rjabber.pop(key)]


	def availableJabber(self):
		if len(self._jabber) == 0: 
			print "Нету ни одного доступного подключения\n\tПрограмма завершает работу\n"
			reactor.stop()
			return False
		return True

	def availableReport(self):
		if len(self._report) == 0: 
			print "Нету ни одного доступного репортера\n\tПрограмма завершает работу\n"
			reactor.stop()
			return False
		return True


	def getRandomReport(self):
		if not self.availableReport(): 
			return False
		if len(self._rreport) == 0:
			self._rreport = self._report[:]

		key = random.randrange(0,len(self._rreport)) if len(self._rreport) > 1 else 0
		print "self._rreport",self._rreport
		print "getRandomReport jids[%d] = %s" % (key,self._rreport[key])
		return self._rreport.pop(key)



			
def signalhandler(*args):
	reactor.stop()
signal.signal(signal.SIGUSR1, signalhandler)
signal.signal(signal.SIGHUP, signalhandler)

if __name__ == "__main__":
	
	def error(msg):
		print '\n\t\t%s\n\t\t%s sample.cfg\n' % (msg,sys.argv[0])
		sys.exit()
	try:
		f = file(sys.argv[1] if 1 in sys.argv else sys.argv[0].replace('.py','.cfg'))
	except IOError:
		error("файл не найдет")
	config = Config(f)
	
        if  "informer" not in config:
                error("Не верный файл конфигурации, отсутствует настройки: source и/или jabber")
        try:
                for informer in config.informer:
                        Informer(informer)
	except:
		print "Exception in user code:"
		print '-'*60
		traceback.print_exc(file=sys.stdout)
		print '-'*60
		error("не понятная ошибка при запуске")
	try:
		fp = open(config.pid,'r')
		pid = fp.read()
		os.kill(int(pid), signal.SIGUSR1)
		fp.close()
	except:
		pass
	if "debug" not in config: 
		pid = os.fork()
		if  pid  == 0:
			class NullDevice:
				def write(self, s):
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
	os.setgid(config.gid)
	os.setuid(config.uid)
	reactor.run()


