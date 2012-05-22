#!/usr/bin/python

from pycurl import *
import urllib,string,socket,StringIO,re

m = CurlMulti()

c = Curl()
b = StringIO.StringIO()
c.setopt(URL, "http://free-advertise.net/")
c.setopt(HTTPHEADER, ["User-Agent: Mozilla/5.001 (windows; U; NT4.0; en-us) Gecko/25250101", "Agent: "])
c.setopt(WRITEFUNCTION, b.write)
c.setopt(CONNECTTIMEOUT, 3)
c.setopt(TIMEOUT, 10)
c.setopt(FOLLOWLOCATION, 0)
c.setopt(MAXREDIRS, 0)
c.data=b;
m.add_handle(c)

while 1:
     ret, num_handles = m.perform()
     if ret != E_CALL_MULTI_PERFORM: break
print "ddd"