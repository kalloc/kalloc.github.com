#!/bin/bash
path=/tmp/core
if [ ! -d ${path} ];then
mkdir ${path}
echo ${path}/core >  /proc/sys/kernel/core_pattern
chmod 777 ${path}
fi
wake=""
if [ -f ${path}/wakeupmylord.py ];then
    wake=${path}/wakeupmylord.py
elif [ -f ~/wakeupmylord.py ];then
    wake=~/wakeupmylord.py
elif [ -f /root/wakeupmylord.py ];then
    wake=/root/wakeupmylord.py
fi
dir=$(date "+%Y-%m-%d_%H.%M.%S")

cd ${path} && ls core* 2>/dev/null |while read core;do
	#уведомляем кодера о ошибке посредством электронной связи
	cmd=$(strings $core  | tail -n1)

	if [ ! -f  /tmp/.gdb.$(basename $cmd) ] || [ $(( $(date +%s) - $( date +%s -r /tmp/.gdb.$(basename $cmd)))) -gt 600 ];then 
	    if [ "$wake" != "" ];then

		echo "bt full" > /tmp/.gdb.$(basename $cmd)
		gdb -batch -x /tmp/.gdb.$(basename $cmd) ${cmd} $path/$core |iconv -c -s -f koi8-r -t utf8 > ${path}/backtrace.txt
		dir=$(date "+%Y-%m-%d_%H.%M.%S")
	$wake nickname@ainmarh.com "
my lord, ${cmd} now crashed :(
lucky news, i'm try to restart (if crash repeatly you be flooded ;)
----------------------*stacktrace*----------------------------
$(cat ${path}/backtrace.txt)
-----------------------------------------------------------------------
"	
	    fi
	else
		gdb -batch -x /tmp/.gdb.$(basename $cmd) ${cmd} $path/$core |iconv -c -s -f koi8-r -t utf8 > ${path}/backtrace.txt
	fi
	#архивируем кору
	cp $cmd ${path}/
	tar zcf ${path}/${dir}.tar.gz  $core $(basename $cmd) backtrace.txt  -C ${path}/
	#зачищаем
	rm -rf ${path}/$core  $path/$(basename $cmd) ${path}/backtrace.txt
done

