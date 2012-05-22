#!/usr/bin/env python
# ainmarh.lab and project daedalus.ru

import socket,os
from twisted.internet import epollreactor
epollreactor.install()
from twisted.names import common,server,dns,hosts,authority
from twisted.application import service, internet
from twisted.internet import defer,reactor
from twisted.python import failure

class NoFileAuthority(authority.FileAuthority):
    def __init__(self, soa, records):
        common.ResolverBase.__init__(self)
        self.soa, self.records = soa, records

class DNSServer(object):
    zones = []
    soa={}

    def __init__(self):
        self.set_soa()
        pass

    def set_soa(self, serial = 100, refresh = 1234, minimum = 7654, expire = 19283784,retry = 15,ttl=1):
        self.soa['serial']=serial
        self.soa['refresh']=refresh
        self.soa['minimum']=minimum
        self.soa['expire']=expire
        self.soa['retry']=retry
        self.soa['ttl']=ttl


    def add_zone(self,host,zones):
        soa_record = dns.Record_SOA(
                    mname = host,
                    rname = 'postmaster.'+host,
                    serial = self.soa['serial'],
                    refresh = self.soa['retry'],
                    minimum = self.soa['minimum'],
                    expire = self.soa['expire'],
                    retry = self.soa['retry'],
                    ttl=self.soa['ttl']
                )
        if '@' in zones:
             zones[host]=zones['@']
             del zones['@']
        if host in zones:
             zones[host].insert(0,soa_record)

        self.zones.append(NoFileAuthority(
             soa = (host, soa_record),
             records = zones
        ))

    def run(self,port=5354):
        factory = server.DNSServerFactory(None , None,self.zones)
        protocol = dns.DNSDatagramProtocol(factory)
        reactor.listenUDP(port, protocol)
        reactor.run()

if __name__ == "__main__":
    srv = DNSServer()
    srv.set_soa(serial = 10000,ttl=666)
    srv.add_zone('yandex.ru',{
        '@': [
            dns.Record_A('213.180.204.11'),
            dns.Record_A('77.88.21.11'),
            dns.Record_A('87.250.251.11'),
            dns.Record_A('93.158.134.11'),
            dns.Record_NS('ns1.yandex.ru'),
            dns.Record_NS('ns2.yandex.ru'),
            dns.Record_NS('ns3.yandex.ru'),
            dns.Record_NS('ns4.yandex.ru'),
            dns.Record_MX(10, 'mx.yandex.ru'),
        ],
        'mx.yandex.ru': [
            dns.Record_A('93.158.134.89'),
            dns.Record_A('213.180.204.89'),
            dns.Record_A('77.88.21.89')
        ],
        'ww.yandex.ru': [
            dns.Record_CNAME('www.yandex.ru')
        ],
        'www.yandex.ru': [
            dns.Record_CNAME('yandex.ru')
        ]
    })
    if (os.fork() == 0): srv.run(5355)




#EOC
