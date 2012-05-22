#!/bin/bash

GW_IP=$(grep remote /etc/openvpn/client.conf   | awk '{print $2}')
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

