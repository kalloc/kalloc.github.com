]#!/bin/bash
check_file='/tmp/last_check'
control_file='/vz/root/765/etc/httpd/conf/httpd.conf'
is_update=false
if [ ! -f "$check_file" ] || [ $(date +%s -r ${control_file}) -gt $(date +%s -r ${check_file}) ];then
    is_update=true
fi


if $is_update ;then
    sed 's|217.171.66.158|*|g' -i ${control_file} 
    /usr/sbin/vzctl set 765 --save --tcpsndbuf 1000000000000 --numtcpsock 200000000 --rate eth0:1:50240
    /usr/sbin/vzctl exec 765 'apachectl restart'
    touch ${check_file}
fi
