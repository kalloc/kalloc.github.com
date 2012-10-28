#!/usr/bin/python
# -*- coding: utf8 -*-

import sys 
import cStringIO
import random
import string
import re
import os
import codecs
import csv


offset=0
user=[]


"prx_num=random.randint(0,len(proxy_list)-1)"



reader = csv.reader(open("ljlogin.txt", "r"),dialect=csv.excel)

writer = csv.writer(open("ljlogin40k.txt", "w"),dialect=csv.excel)

for row in reader:  user.append(row)

num=len(user)

for c in range(0,40000,1):
    if num == 0: break
    r=random.randint(0,num)-1
    row=user[r]
    del user[r]
    writer.writerow(row)
    num=num-1
