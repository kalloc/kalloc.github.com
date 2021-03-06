#!/bin/bash
export DISPLAY=":0"

if [ $# -lt 2 ];then
	echo -ne "ошибка, не указан $( [ $# == 0 ] && echo "логин и пароль" || echo "пароль").\nзапускать надо вот так:\t$0 ваш_логин ваш_пароль\n\n\t\t\t\t\tс любовью, дедал и сыновья\n"
	exit
fi
LOGIN="$1"
PASS="$2" 
JAR=/tmp/${LOGIN}_solomonitoring.ru.jar 
need_login=false
wrong_login=true

#проверяет, а есть ли в деревне инет
ping ya.ru -w 3 -c 1 2>&1 >/dev/null
if [ $? != 0 ];then
	notify-send -u critical "Internet is end" "интернет закончился. срочно завозите. народ бунтует.";
	exit
fi


if [ ! -f ${JAR} ];then
	need_login=true
else 

	which notify-send > /dev/null
	if [ $? != 0 ];then
			echo -ne "ошибка, не установлен notify-send\nозаботьтись этим\n"
			exit
	fi
fi



#пробуем логиниться
if  $need_login;then
	curl -b 'onlist=100; filterby_objects=all; groupby_noticescontact=groups; hidehelp[0]=1; groupby_objects=groups; hidehelp[7]=1; language=ru;' -c ${JAR}  -d "go=GO&login=${LOGIN}&password=${PASS}" http://admincp.solomonitoring.ru/index.php  -is | grep  "302 Found"
	if [ $? != 0 ];then
		notify-send -u critical "Solomonitoring.Ru" "не удалось авторизоваться, наверное это плохо.";
		exit
	fi
fi


result=$(curl -is -b  ${JAR} -d"ajaxmain=gettablecurrentissue&randomval=$(date +%s)" http://admincp.solomonitoring.ru/ajax/ajax_main_summary.php -e http://admincp.solomonitoring.ru/index.php) 
echo "$result" | grep Set-Cookie >/dev/null
if [ $? == 0 ];then
	#rm -f ${JAR}
	exit
fi



ascii=$(echo "$result" | grep -A1000 '<tr>' | html2text -utf8 | grep '(' | perl  -e '$x=0;while(<>) {s/\([^)]+[a-z]+\)//;s/\n//g;print $_;print "\n" if $x%2;print " - " if $x%2 == 0;$x++;}' )
if [ "$ascii" != "" ];then
		notify-send -u critical "Current downtime report" "$ascii"
fi

