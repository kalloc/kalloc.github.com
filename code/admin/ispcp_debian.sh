#!/bin/bash

#ainmarh.com ispcp installer


function get_ip {

    echo "";
    for ip in `ifconfig  | grep inet | awk '{print $2}' | awk -F: '{print $2}' | grep -v 127.0.0`;do
    echo "	    $ip";
    done;

}


function die {

    if [ $? != 0 ]; then 
	echo $@;  
	exit
    fi

}

hostname | grep '.asprex.ru' > /dev/null 2>&1
die propishite hostname k vidu chto-toNUM.asprex.ru

hostname=`hostname 2>/dev/null`

ping -c 1 $hostname  > /dev/null 2>&1
die hostname - $hostname, ne ping\'uetsa


echo "deb http://mirror.yandex.ru/debian/ lenny main non-free contrib
deb http://security.debian.org/ lenny/updates main non-free contrib
" > /etc/apt/sources.list
apt-get update
apt-get upgrade
apt-get install  apache2 makepasswd apache2-mpm-worker apache2 apache2.2-common apache2-utils awstats bash bind9 bzip2 courier-authdaemon courier-base courier-imap courier-maildrop courier-pop chkrootkit diff dnsutils gcc gzip iptables ispell  libapache2-mod-fcgid libberkeleydb-perl libc6-dev libcrypt-blowfish-perl libcrypt-cbc-perl libcrypt-passwdmd5-perl libdate-calc-perl libdate-manip-perl libdbd-mysql-perl libdbi-perl libio-stringy-perl libmail-sendmail-perl libmailtools-perl libmcrypt4 libmd5-perl libmime-perl libnet-dns-perl libnet-libidn-perl libnet-netmask-perl libnet-smtp-server-perl  libsnmp-session-perl libterm-readkey-perl libterm-readpassword-perl libtimedate-perl libmysqlclient15off locales lsb-base make mysql-client mysql-common mysql-server original-awk patch perl perl-base perl-modules php5-cgi php5-gd php5-mcrypt php5-mhash php5-mysql policyd-weight postfix postgrey procmail  rkhunter sasl2-bin ssh tar wget  bind9 bzip2 courier-authdaemon courier-base courier-imap courier-maildrop courier-pop diff dnsutils gcc gzip iptables libberkeleydb-perl libc6-dev libcrypt-blowfish-perl libcrypt-cbc-perl libcrypt-passwdmd5-perl libdate-calc-perl libdate-manip-perl libdbd-mysql-perl libdbi-perl libio-stringy-perl libmail-sendmail-perl libmailtools-perl libmcrypt4 libmd5-perl libmime-perl libnet-dns-perl libnet-netmask-perl libnet-smtp-server-perl  libsasl2-modules libsnmp-session-perl libterm-readkey-perl libterm-readpassword-perl libtimedate-perl make mysql-client mysql-common mysql-server original-awk patch perl perl-base perl-modules php5 php5-gd php5-mcrypt php5-mysql php5-cgi php-pear postfix procmail  sasl2-bin ssh tar wget mc pwgen subversion cron apache2-suexec  libperl5.10  libsasl2-2 php-pear php5 php5-adodb  proftpd
die apt-get error install

update-rc.d cron defaults 30 

if [ `df -a /tmp/ | grep / |awk '{print $4}'` -le 4096 ];then
    echo /tmp menshe 4096kb
    exit
fi;



sed -i 's/\(UseIPv[6\t]\+\)on//' /etc/proftpd/proftpd.conf
sed -i 's/\(UseIPv[6\t]\+\)on//' /etc/proftpd/proftpd.conf
sed -i 's/LoadModule mod_sql_postgres\.c/#\0/g' /etc/proftpd/modules.conf
sed -i 's/LoadModule mod_ldap\.c/#\0/g' /etc/proftpd/modules.conf
rm -f /etc/proftpd.conf
ln -s  /etc/proftpd/proftpd.conf  /etc/proftpd.conf
/etc/init.d/proftpd restart

if [ ! -f /root/.my.cnf ];then

    passdb=`pwgen -n 12 1`
    admin=`pwgen -n 12 1`

    echo "

[client]
default-character-set=utf8

[mysqld]
default-character-set=utf8
init-connect='SET NAMES utf8'
init-connect='SET CHARACTER SET utf8'
skip-character-set-client-handshake
" > /etc/mysql/conf.d/russian.cnf

mysqladmin password $passdb
die set sql password error

echo "[client]
password=$passdb
" > /root/.my.cnf

echo "AddDefaultCharset cp1251" > /etc/apache2/conf.d/charset
apache2ctl restart > /dev/null 2>&1

echo $admin > /root/.ispcp2.pass

else

    passdb=`cat /root/.my.cnf | grep password | awk -F'=' '{print $2}' `
    admin=`cat /root/.ispcp2.pass`


fi;

/etc/init.d/mysql restart  > /dev/null 2>&1


cd /root
rm -rf /root/ispcp

wget -O -  http://virtualserver.ru/conf/ispcp.tbz2  | tar -jxf -
cd ispcp
die unable to enter ispcp

sed  -i "s/DATABASE_PASSWORD =/\0$passdb/" configs/debian/ispcp.conf
make
die unable to make
make install
die unable to make install
cd /tmp/ispcp
cp -R * /
die unable to cp -R * /
rootph=`makepasswd --crypt-md5 --chars=10`
rooth=`echo $rootph | awk '{print $2}'`
rootp=`echo $rootph | awk '{print $1}'`
usermod -p $rooth root
echo "admin pass: $admin"
echo "db pass: $passdb"
echo "IP:"
get_ip

cd /var/www/ispcp/engine/setup
perl ./ispcp-setup


echo "
nameserver 213.248.0.6
nameserver 213.248.1.6
" > /etc/resolv.conf


postconf -e "mynetworks_style = subnet"
postconf -e "myhostname = mail.$hostname"
postconf -e "mydomain = mail.$hostname"
postconf -e 'mynetworks = $mydomain,$myhostname,127.0.0.1'

/etc/init.d/postfix restart

sed -i 's/LoadModule mod_sql_postgres\.c/#\0/g' /etc/proftpd/modules.conf
sed -i 's/LoadModule mod_ldap\.c/#\0/g' /etc/proftpd/modules.conf

sed -e -i 's/#(LoadModule mod_sql\.c)/\1/g' /etc/proftpd/modules.conf
sed -e -i 's/#(LoadModule mod_sql_mysql\.c)/\1/g' /etc/proftpd/modules.conf
sed -e -i 's/#(LoadModule mod_quotatab_sql\.c)/\1/g' /etc/proftpd/modules.conf
sed -e -i 's/#(LoadModule mod_wrap2_sql\.c)/\1/g' /etc/proftpd/modules.conf
sed -e -i 's/#(Include \/etc\/proftpd\/modules\.conf)/\1/g' /etc/proftpd/modules.conf
sed -e -i 's/#ServerTokens Minimal/ServerTokens Minimal/' /etc/apache2/conf.d/security
sed -e -i 's/ServerTokens Full/#ServerTokens Full/' /etc/apache2/conf.d/security
sed -e -i 's/#ServerSignature Off/ServerSignature Off/' /etc/apache2/conf.d/security
sed -e -i 's/ServerSignature On/#ServerSignature On/' /etc/apache2/conf.d/security


/etc/init.d/proftpd restart

sed "s/\(ns[12]\).{DMN_NAME}./\1.$hostname\./g" /etc/ispcp/bind/parts/* -i
/etc/init.d/bind9 restart 
a2dismod fcgid
cp /etc/apache2/mods-available/fcgid.load /etc/apache2/mods-available/fcgid_ispcp.load
a2enmod fcgid_ispcp
sed 's/\/etc\/init.d\/apache2 restart/killall -9 apache2;sleep 5;\0/g' -i  /etc/logrotate.d/apache2
echo '*/5 * * * * if [ "`pidof apache2`" == "" ]; then apache2ctl restart; fi' >> /etc/crontab

apache2ctl restart
clear


echo "
Host:       
	    $hostname
	    `host  $hostname | cut -f3`
NS:
	    ns1.$hostname
	    ns2.$hostname
	    
IP:         `get_ip`


SSH:
           Login: root
           Password: $rootp


ispcp:
     url: http://`hostname`/

     Admin (полный доступ , создание ресселеров)
           Login: admin
           Password: $admin
     Resseler (ограниченный доступ , создание сайтов)
           Login: admin_r
           Password: $admin
	   
MYSQL:
           Login: root 
           Password: $passdb
	
"  > /root/.info
cat /root/.info
