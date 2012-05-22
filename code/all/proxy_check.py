#!/usr/bin/python

from pycurl import *
import urllib,string,socket,StringIO,re


def http_error_401(self, url, fp, errcode, errmsg, headers, data=None):
    """Error 401 -- also relocated (permanently)."""
    return self.http_error_401(url, fp, errcode, errmsg, headers, data)
urllib.http_error_401 = http_error_401
		
socket.setdefaulttimeout(1)
proxy_list=[]




f = file("proxy.txt", 'r')
pool=[]
for line in f.read().split("\n"):
	if re.match("""[^:]+:[0-9]+""",line	):
	    try:
		c = Curl()

		b = StringIO.StringIO()
		proxy=line.strip()
    	        proxies = {'http': 'http://'+proxy}
	        c.setopt(URL, "http://www.example.com/")
	        c.setopt(HTTPHEADER, ["User-Agent: Mozilla/5.001 (windows; U; NT4.0; en-us) Gecko/25250101", "Agent: "])
	        (host,port)=proxy.split(":")
	        c.setopt(PROXY, host)
	        c.setopt(PROXYPORT,int(port))
	        c.setopt(WRITEFUNCTION, b.write)
	        c.setopt(TIMEOUT, 1)
	        c.setopt(FOLLOWLOCATION, 0)
	        c.setopt(MAXREDIRS, 0)
	        c.perform()
		data=b.getvalue()
		dd=re.findall("""http://www.rfc-editor.org/rfc/rfc2606.txt""",data)
		 
		if dd:
		    proxy_list.append(proxy)
		    print "PROXY",proxy
		else:
    		    print "NOPRY",proxy
	    except:
		print "BADIP",proxy
		pass	    

f.close()


f = file("proxy.txt",'w')
for line in proxy_list:
    f.write(line+"\n")
f.close()


