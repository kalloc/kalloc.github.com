#!/bin/bash
BASE=`vgdisplay  2>&1 | grep "VG Name" | awk '{print $3}'`
if [ $# -lt 2 ];then
echo "usage $0 user ip"
exit
fi
#PREFIX
export HOSTNAME=""
export NAME=$1
export IP=$2

if [ -d /etc/vservers/${NAME} ];then
    echo already exists user ${NAME}
    exit
fi

export ARCH=$(test $(uname -m) == x86_64 && echo amd64 || echo i386);


function LVM() {
	read -p "Использовать LVM? [Yes/No]: " ret
	if [[ $ret =~ ^Y|y ]]; then
	    export islvm=true
	    test -f /dev/$BASE/$NAME && (printf "\e[1;31m\t\t\t LVM %s уже есть\e[0m\n" $NAME;exit)
	fi
}


function DIST() {
    read -p "Укажите дистрибутив [lenny/squeeze/empty]: " ret
    if [[ $ret == "lenny" ]];then
	dist=lenny
    elif [[ $ret == "squeeze" ]];then
	dist=squeeze
    elif [[ $ret == "empty" ]];then
	dist=empty
    else
	printf "\e[1;31m\t\t\t Не известный тип дистрибутива\e[0m\n";exit
    fi
    export dist=$dist

}

function LVM_SIZE() {
read -p "Укажите размер LVM в гигабайтах: " ret
export lvm_size=$ret
}


function RSS() {
read -p "Укажите размер Памяти в МБ: " ret
export rss=$(( $ret * 1024 / 4  ))
}


function create() {
mkdir /vservers/${NAME};
if [[ $islvm ]];then
    test -f /dev/$BASE/${NAME} && (printf "\e[1;31m\t\t\t LVM - %s уже существует в системе\e[0m\n" $NAME;exit)
    lvcreate -L${lvm_size}G -n ${NAME} $BASE >/dev/null  2>&1|| (printf "\e[1;31m\t\t\t Не удалось создать LVM \e[0m\n";rm -rf /vservers/${NAME};exit)
    if [[ -d /vservers/${NAME} ]];then
	mkfs.xfs /dev/$BASE/${NAME} >/dev/null  2>&1
	mount /dev/$BASE/${NAME} /vservers/${NAME}  >/dev/null  2>&1 || (printf "\e[1;31m\t\t\t Не удалось смонтировать\e[0m\n";lvremove -f $BASE/$NAME > /dev/null 2>&1 ; rm -rf /vservers/${NAME};exit)
    fi

    if [[ -d /vservers/${NAME} ]];then
	echo "/dev/$BASE/${NAME}    /vservers/${NAME}   xfs   noatime       0       1" >> /etc/fstab
    fi
fi
if [[ -d /vservers/${NAME} ]];then
    if [[ $dist == "empty" ]];then
    vserver  ${NAME} build  --force -m skeleton -n${NAME} --hostname ${NAME}${HOSTNAME} \
    --flags lock,nproc,ulimit,sched_prio,virt_load,virt_cpu,virt_uptime,virt_mem \
    --interface "eth0:${IP}" 
    else
    vserver  ${NAME} build  --force -m debootstrap -n${NAME} --hostname ${NAME}${HOSTNAME} \
    --flags lock,nproc,ulimit,sched_prio,virt_load,virt_cpu,virt_uptime,virt_mem \
    --interface "eth0:${IP}"  -- -d $dist
    echo -ne "deb http://mirror.yandex.ru/debian/ $dist main contrib non-free\ndeb	http://mirror.yandex.ru/debian-security $dist/updates main non-free contrib\n" > /vservers/${NAME}/etc/apt/sources.list
    rm -f /vservers/${NAME}/etc/localtime ; ln -s /usr/share/zoneinfo/Europe/Moscow /vservers/${NAME}/etc/localtime

    fi
    echo "default" > /etc/vservers/${NAME}/apps/init/mark
    fstab=`grep -v tmpfs  /etc/vservers/${NAME}/fstab`
    echo "$fstab" > /etc/vservers/${NAME}/fstab
    mkdir /etc/vservers/${NAME}/rlimits;
    echo $rss > /etc/vservers/${NAME}/rlimits/rss;
    echo "2.6.33" > /etc/vservers/${NAME}/uts/release;
    echo "Linux" > /etc/vservers/${NAME}/uts/sysname;
    echo "Daedalus.ru" > /etc/vservers/${NAME}/uts/version;
fi

}


function confirm() {
    printf "\e[1;33m Выбрана Следующая конфигурация:\e[0m\n" 
    printf "\e[1;34m\t Host  : %s\e[0m\n" $NAME$HOSTNAME
    printf "\e[1;34m\t IP  : %s\e[0m\n" $IP
    printf "\e[1;34m\t VPS : %s\e[0m\n" $NAME
    printf "\e[1;34m\t RAM : %sM \e[0m\n" $(($rss/1024*4)) 
    printf "\e[1;34m\t Dist : %s\e[0m\n" $(if [[ $dist == "empty" ]];then echo "empty"; else echo $dist;fi)
    printf "\e[1;34m\t LVM : %s\e[0m\n" $(if [[ $islvm ]];then echo "ДА"; else echo "НЕТ";fi)
    if [[ $islvm ]];then 
	printf "\e[1;34m\t LVM Size : %sG\e[0m\n" $lvm_size
    fi
    read -p "Добавить? [Yes/No]: " ret
    if [[ ! $ret  =~ ^Y|y ]];then
        exit
    fi
}


LVM
RSS
DIST
if [[ $islvm ]];then 
    LVM_SIZE
fi

confirm
create



