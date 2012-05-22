#!/bin/bash
echo -n enter gateway ip: 
read GW_IP
echo you entered $GW_IP, continue?
echo press enter or ctrl+c for cancel
read

CURRENT_DIR=$(pwd)
ARCH=$(uname -p)

echo uninstall pptpd
yum remove pptpd -y
echo install requirements packages
yum install -y wget gcc openssl-devel lzo-devel make perl ppp || exit

echo install pptpd
(rpm -Uhv http://poptop.sourceforge.net/yum/stable/packages/pptpd-1.3.4-2.el6.${ARCH}.rpm)  || exit

echo install openvpn
(wget -q -O - http://swupdate.openvpn.org/community/releases/openvpn-2.2.2.tar.gz | tar zxf - -C /tmp && cd /tmp/openvpn* && ./configure && make install && cd / && rm -rf /tmp/openvpn-*) || exit

echo configure link between server A and server B

mkdir /etc/openvpn

cat > /etc/openvpn/openssl.cfg <<EOF
[ ca ]
basicConstraints			= critical,CA:TRUE
nsCertType = objsign,sslCA,emailCA
keyUsage = digitalSignature,nonRepudiation,keyCertSign,cRLSign
nsCertType = sslCA, emailCA
subjectKeyIdentifier = hash
[ server ]
basicConstraints			= CA:FALSE
authorityKeyIdentifier=keyid:always,issuer:always
subjectKeyIdentifier = hash
keyUsage = critical, nonRepudiation, digitalSignature, keyEncipherment, dataEncipherment
nsCertType = server, email, objsign
extendedKeyUsage = serverAuth,nsSGC,msSGC
[ client ]
authorityKeyIdentifier=keyid:always,issuer:always
subjectKeyIdentifier = hash
basicConstraints			= CA:FALSE
keyUsage = critical, nonRepudiation, digitalSignature, keyEncipherment, dataEncipherment
nsCertType = client, email, objsign
[ req ]
distinguished_name = req_distinguished_name
prompt = no
[ req_distinguished_name ]
CN=EU
ST=Stambul
L=Madagascar
CN=Certificate Authority

EOF

echo 01 > /etc/openvpn/ca.srl

echo create CA key pair
openssl genrsa 1024 > /etc/openvpn/ca.key
openssl req -new -key /etc/openvpn/ca.key  -days 3650 -batch  -subj '/CN=Certificate Authority'  | openssl x509 -req  -signkey /etc/openvpn/ca.key -days 3650 -extensions ca -extfile /etc/openvpn/openssl.cfg -text >/etc/openvpn/ca.crt

echo create server B key pair
openssl genrsa 1024 > /etc/openvpn/server.pem
openssl req -new -key /etc/openvpn/server.pem   -batch  -subj '/CN=Server'  | openssl x509 -req  -CA /etc/openvpn/ca.crt -CAkey /etc/openvpn/ca.key -CAserial /etc/openvpn/ca.srl -days 3650  -text -extensions server -extfile /etc/openvpn/openssl.cfg  >>/etc/openvpn/server.pem
 
echo create client A key pair
openssl genrsa 1024 > /etc/openvpn/client.pem
openssl req -new -key /etc/openvpn/client.pem   -batch  -subj '/CN=Client'  | openssl x509 -req  -CA /etc/openvpn/ca.crt -CAkey /etc/openvpn/ca.key -CAserial /etc/openvpn/ca.srl -days 3650 -text -extensions client -extfile /etc/openvpn/openssl.cfg  >>/etc/openvpn/client.pem

echo create client config

cat > /etc/openvpn/client.conf <<EOF
client
dev tun 
remote $GW_IP
proto udp 
persist-tun 
persist-key
cipher AES-128-CBC 
comp-lzo 
verb 4 
ns-cert-type server 
nobind 
tls-client 
topology subnet
<ca>
$(openssl x509 < /etc/openvpn/ca.crt)
</ca>
<cert>
$(openssl x509 < /etc/openvpn/client.pem)
</cert>
<key>
$(openssl rsa < /etc/openvpn/client.pem)
</key>
status /etc/openvpn/openvpn-status.log
log /etc/openvpn/openvpn.log
verb 4

EOF
echo create server config

cat > /etc/openvpn/server.conf <<EOF
dev tun
mode server
topology subnet
port 1194
proto udp
#link-mtu 1442
tun-mtu 1500
mssfix 1400
cipher AES-128-CBC
ifconfig 10.0.0.1 255.255.128.0
ifconfig-pool 10.0.0.2 10.0.128.255
ifconfig-pool-persist ipp.txt
<ca>
$(openssl x509 < /etc/openvpn/ca.crt)
</ca>
<cert>
$(openssl x509 < /etc/openvpn/server.pem)
</cert>
<key>
$(openssl rsa < /etc/openvpn/server.pem)
</key>
dh /etc/openvpn/dh.pem
persist-key
persist-tun
keepalive 10 60
duplicate-cn
client-to-client
comp-lzo
tls-server
status /etc/openvpn/openvpn-status.log
log /etc/openvpn/openvpn.log
verb 4

client-config-dir ccd
EOF

echo configure pptpd

cat > /etc/pptpd.conf <<EOF
option /etc/ppp/pptpd-options
connections 2000
localip 10.1.0.1
remoteip 10.1.0.2-254
EOF
cat > /etc/ppp/pptpd-options <<EOF
#version 7
refuse-pap
refuse-eap
refuse-chap
refuse-mschap
require-mppe
require-mschap-v2
require-mppe-128
ms-dns 8.8.8.8
ipcp-accept-local
ipcp-accept-remote
lcp-echo-failure 30
lcp-echo-interval 5
nodefaultroute
lock
#nobsdcomp
#noipx
mtu 1450
mru 1450
EOF


cat > /etc/ppp/ip-down.local <<EOF 
#!/bin/sh
ip rule del from \$IPREMOTE table 1000
EOF

cat > /etc/ppp/ip-up.local  <<EOF
#!/bin/sh
tun=\$(ifconfig  | grep tun | awk '{print \$1}')
ip route show table all | grep 1000 > /dev/null || ip route add default table 1000  via 10.0.0.1  dev \$tun
ip rule add from \$IPREMOTE table 1000
EOF
chmod +x /etc/ppp/ip-up.local /etc/ppp/ip-down.local

echo test login - login with password - password
cat > /etc/ppp/chap-secrets <<EOF
login * password *

EOF

#echo onboot start pptpd
chkconfig --add pptpd
chkconfig   pptpd on

# start pptpd
/etc/init.d/pptpd start
#init on boot
cat > /etc/rc.local <<EOF
#!/bin/bash
modprobe ip_gre
modprobe ip_nat_pptp
modprobe tun
modprobe pppol2tp
sysctl net.ipv4.ip_forward=1
iptables -F
iptables -F -t nat
iptables -t nat -A POSTROUTING  -s 10.1.0.0/24 -o tun+ -j MASQUERADE
/usr/local/sbin/openvpn  --daemon --config /etc/openvpn/client.conf
touch /var/lock/subsys/local
EOF

#restart tunnel
cat > /etc/cron.d/openvpn << EOF
* * * * * root pgrep openvpn > /dev/null || /usr/local/sbin/openvpn --daemon --config /etc/openvpn/server.conf
EOF


cd $CURRENT_DIR
echo place /etc/openvpn/server.conf to $GW_IP and run /etc/rc.local 





