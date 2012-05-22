#!/bin/bash

function promtLogin() {
    read -p "введите желаемый логин: "  login
    isNew=$(id $login > /dev/null 2>&1 || echo true )
    if [[ $isNew ]];then
	printf "\e[1;34m\t Будет создан новый логин: %s\e[0m\n" $login
    fi
    export isNew="$isNew"
}

function promtHost() {
    read -p "введите желаемый домен: "  domain
    db=$(echo "$domain" | sed 's/[-_.]//g')
    export db=${db:0:10}
}

function YesNo() {
    read -p "Добавить $1? [Yes/No]: " ret
    if [[ $ret =~ "^Y|y" ]]; then
	export $2=true
    else
    	export $2=""
    fi
}

function genMySQL() {
    export passSQL=$(pwgen 12 1)
}

function genFTP() {
    string=$(makepasswd  --minchars=16 --maxchars=20 --crypt-md5)
    export passFTP=$(echo $string | awk '{print $1} ')
    export md5=$(echo $string | awk '{print $2}' )
}


function confirm() {
    printf "\e[1;33m Выбрана Следующая конфигурация:\e[0m\n" 
    printf "\e[1;34m\t Host  : %s\e[0m\n" $domain
    printf "\e[1;34m\t Login : %s\e[0m\n" $login
    if [[ $isDNS ]] && [[ ! -f /etc/bind/master/$domain ]];then
	printf "\e[1;35m\t\t DNS:\t%s\e[0m\n" $(if [[ $isDNS ]];then echo "ДА"; else echo "НЕТ";fi)
	printf "\e[1;35m\t\t GMAIL:\t%s\e[0m\n" $(if [[ $isGMAIL ]];then echo "ДА"; else echo "НЕТ";fi)
    else
	if [[  -f /etc/bind/master/$domain ]];then
	    printf "\e[1;31m\t\t\t DNS %s уже есть\e[0m\n" $domain
	fi
	isDNS="";
    fi


    if [[ $isHTTP ]] && [[ ! -f /etc/nginx/conf.d/$domain ]];then
	printf "\e[1;35m\t\t HTTP:\t%s\e[0m\n"  $(if [[ $isHTTP ]];then echo "ДА"; else echo "НЕТ";fi)
	if [[ $isNew ]];then
	    genFTP
	    printf "\e[1;34m\t\t\t FTP password  : %s ( ftp://%s:%s@88.212.197.50/%s )\e[0m\n" $passFTP $login $passFTP $domain
	fi
    else
	if [[ -f /etc/nginx/conf.d/$domain ]];then
	printf "\e[1;31m\t\t\t HTTP %s уже есть\e[0m\n" $domain
	fi
    	isHTTP="";
    fi


    if [[ $isSQL ]]  && [[ ! -d /var/lib/mysql/$db ]];then
	printf "\e[1;35m\t\t MySQL:\t%s\e[0m\n" $(if [[ $isSQL ]];then echo "ДА"; else echo "НЕТ";fi)
	genMySQL
	printf "\e[1;34m\t\t\t SQL login/db  : %s\e[0m\n" $db 
	printf "\e[1;34m\t\t\t SQL password  : %s\e[0m\n" $passSQL 
    else
	if [[ -d /var/lib/mysql/$db ]];then
	printf "\e[1;31m\t\t\t SQL %s уже есть\e[0m\n" $db
	fi
	isSQL="";
    
    fi
    
    
    
    
    read -p "Добавить? [Yes/No]: " ret
    
    if [[ ! $ret  =~ "^Y|y" ]];then
	exit
    fi
}


function create() {
    if [[ $isNew ]];then
	useradd -m -s /bin/false -d /home/$login -p"$md5" $login
	touch /etc/apache2/sites-available/$login
	a2ensite $login
    fi
    
    if [[ $isHTTP ]];then
	mkdir /home/$login/$domain/{www,tmp,logs} -p
	chown -R $login.$login /home/$login/$domain/
	chmod -R 750 /home/$login/$domain/
	chmod 770 /home/$login/$domain/tmp/
	echo "Use VirtualHost $domain www.$domain /home/$login/$domain /home/$login/$domain/www on" >>/etc/apache2/sites-available/$login;
	echo '
server {
    listen          88.212.204.110:80;
    server_name     .'$domain';
    include	    /etc/nginx/inc/pixel.conf;
    include         /etc/nginx/common-proxy.conf;
    
    location ~* ^.+\.(jpg|jpeg|gif|png|ico|css|zip|tgz|gz|rar|bz2|doc|xls|exe|pdf|ppt|txt|tar|wav|bmp|rtf|js|avi|js|mov|mpeg|mpg|mp3|swf|vob|xml|3gp|torrent)$ {
        root    /home/'$login'/'$domain'/www;
	try_files $uri @proxy;
    }
}
' > /etc/nginx/conf.d/$domain
    killall -HUP nginx
    apache2ctl restart

    fi
    
    if [[ $isSQL ]];then
        echo "create database $db;grant all on $db.* to $db@localhost identified by '$passSQL';flush privileges" | mysql -q;
    fi

    if [[ $isDNS ]];then
	if [[ $isGMAIL ]];then
	    MX="
@		IN MX 10 ASPMX.L.GOOGLE.COM.
@		IN MX 20 ALT1.ASPMX.L.GOOGLE.COM.
@		IN MX 20 ALT2.ASPMX.L.GOOGLE.COM.
@		IN MX 30 ASPMX2.GOOGLEMAIL.COM.
@		IN MX 30 ASPMX3.GOOGLEMAIL.COM.
@		IN MX 30 ASPMX4.GOOGLEMAIL.COM.
@		IN MX 30 ASPMX5.GOOGLEMAIL.COM.
	    
m		IN CNAME ghs.google.com.
    
	    "
	else
	    MX="
@               IN MX 10 @
	    "
	
	fi
echo '$TTL 14440

@       IN SOA ns1.svyatov.com. noc.svyatov.com. (
        2007111101;
        3H;
        1H;
        1W;
        1D
);

                IN NS ns1.svyatov.com.
                IN NS ns2.svyatov.com.

@		IN TXT "v=spf1 a mx ~all"

@               IN A 88.212.204.110
'"$MX"'
*               IN CNAME @
' > /etc/bind/master/$domain

echo '
zone "'$domain'."
{
    type master;
    file	"/etc/bind/master/'$domain'";
};
' >> /etc/bind/named.conf.local	
    rndc reload > /dev/null
    fi

    printf "\e[1;33m Добавлено\e[0m\n" 

}

