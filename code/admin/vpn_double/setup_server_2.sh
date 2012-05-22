#!/bin/bash
#server 2 - Centos 6.0 - x86_64
#change it
CURRENT_DIR=$(pwd)
ARCH=$(uname -p)
yum install -q -y wget lzo-devel openssl-devel make gcc perl ppp  || exit

(wget -q -O - http://swupdate.openvpn.org/community/releases/openvpn-2.2.2.tar.gz | tar zxf - -C /tmp && cd /tmp/openvpn* && ./configure && make install && cd / && rm -rf /tmp/openvpn-*)  || exit


#autorestart
cat > /etc/cron.d/openvpn << EOF
* * * * * root pgrep openvpn > /dev/null || /usr/local/sbin/openvpn --daemon --config /etc/openvpn/server.conf
EOF

#init on boot
cat > /etc/rc.local <<EOF
#!/bin/sh
modprobe ip_gre
modprobe ip_nat_pptp
modprobe tun
modprobe pppol2tp
sysctl net.ipv4.ip_forward=1
iptables -F
iptables -F -t nat
iptables -t nat -A POSTROUTING  -s 10.0.0.0/8 -j MASQUERADE
/usr/local/sbin/openvpn --daemon --config /etc/openvpn/server.conf
touch /var/lock/subsys/local
EOF

#openvpn confs
mkdir /etc/openvpn/ccd -p
cat > /etc/openvpn/ccd/Client <<EOF
iroute 10.1.0.0 255.255.255.0
iroute 10.0.0.0 255.255.255.0
EOF

echo gen server A DH key
openssl gendh > /etc/openvpn/dh.pem
cd $CURRENT_DIR
echo place server.conf to /etc/openvpn/ and run /etc/rc.local



