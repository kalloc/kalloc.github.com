#! /bin/sh
#
# Created by configure
apt-get install libbz2-dev bzip2 libcurl4-openssl-dev libfreetype6-dev libxft-dev libt1-dev libc-client2007b-dev libmcrypt-dev

#'./configure' \
'--enable-zend-multibyte' \
'--enable-zip' \
'--with-gd' \
'--enable-fpm' \
'--with-layout=GNU' \
'--with-config-file-scan-dir=/usr/local/etc/php' \
'--with-mysql' \
'--enable-magic-quotes' \
'--with-pear' \
'--with-openssl' \
'--with-zlib' \
'--with-imap-ssl' \
'--with-bz2' \
'--enable-calendar' \
'--with-curl' \
'--enable-dba' \
'--enable-exif' \
'--enable-ftp' \
'--with-imap' \
'--with-mcrypt' \
'--enable-mbstring' \
'--with-kerberos' \
'--with-iconv' \
'--enable-pcntl' \
'--enable-sockets' \
'--with-freetype-dir' \
'--with-jpeg-dir' \
'--with-png-dir' \
'--with-zlib-dir' \
'--with-xpm-dir' \
'--with-t1lib' \
'--enable-gd-native-ttf' \
'--with-jpeg-dir' \
'--with-freetype-dir=/usr/lib' \
"$@"

make 
make install

wget n1ck.name/conf/php-fpm.conf -O /usr/local/etc/php-fpm.conf
echo "" >> /etc/rc.local
echo "/usr/local/sbin/php-fpm start" >> /etc/rc.local

