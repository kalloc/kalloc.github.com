#!/bin/bash

dir="/backup/"
echo image size:`du -hs $dir| awk  '{print $1}'`
DATE=`date +%Y-%m-%d_%H-%M`;

if [ -f "$dir/listcurrent.txt" ]; then
    mv $dir/listcurrent.txt $dir/listcurrent_old.txt
fi

cd $dir/current
find ./ -type f > ../listcurrent.txt
cd $dir

if [ -f "$dir/listcurrent_old.txt" ]; then



        diff -urN listcurrent_old.txt listcurrent.txt | grep "^\(+\.\/\|-\.\/\)"  > $dir/$DATE"_new-del".txt
        listnd=`cat $dir/$DATE"_new-del".txt`
        if [ "$listnd" ]; then
            echo "new/delete file in image"
            echo "--------------------------"
                cat  $dir/$DATE"_new-del".txt
            echo "--------------------------"
        else
            rm -f $dir/$DATE"_new-del".txt
        fi
fi


if [  -d "$dir/modified" ]; then
cd $dir/modified
find ./ -type f > $dir/$DATE"_mod".txt
list=`cat $dir/$DATE"_mod".txt`
cd $dir
    if [ "$list" ]; then
        #cd $dir/modified
        echo "backup modified  file now"
        echo "--------------------------"
        cat  $dir/$DATE"_mod".txt
        echo "--------------------------"
        tar -zcf $dir/$DATE.tgz -C $dir/modified/  -T $dir/$DATE"_mod".txt 2>&1 > /dev/null
        echo backuped, size:`ls -alh $dir/$DATE.tgz |awk '{print $5}'`
    else
        rm -f $dir/$DATE"_mod".txt
    fi

    rm -rf $dir/modified

fi



