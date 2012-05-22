if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return 
end




local cIDs='';
local code=QUERY.id..QUERY.hash

local uaid=browser_id()

local src=get_source(code)
if src == nil then
    error404()
    return
end

print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")
print("Content-Type:text/plain;charset=utf-8\r\n\r\n");
print("loading: ")

local dec_ip = string.gsub(http_ip,'^(%d+).(%d+).(%d+).(%d+)$',function (i1,i2,i3,i4) return(i1*(256^3)+i2*(256^2)+i3*256+i4) end )
local key = string.format('ead_%x_%x_%x',dec_ip,uaid,src.id);
local value = mem[1]:get(key)
if value ~= nil then
    value = json.decode(value)
end

local nextday=get_nextday(1)
if type(value) == 'table'  then

    if value.o and value.o.time > (magnet.time-180) then	    

	if table.getn(value.c) == 0 then
	    mysql.query([[insert into stat_day VALUES(NOW(),%d,%d,INET_ATON("%s"),'%s',%d,'true')]],
		value.o.cid,
	        src.id,
		http_ip,
	        browser(http_ua),
		browser_id(http_ua)
	    )
	    src.free=src.free-1
	    mysql.query([[update traf set count=count+1,hit=hit+1 where ts=NOW()  and ((type ='source' and id = %d) or (type ='client' and id = %d))]],src.id,value.o.cid)
	    print (" a ");
	else
	    mysql.query([[insert into stat_day VALUES(NOW(),%d,%d,INET_ATON("%s"),'%s',%d,'false')]],
		value.o.cid,
	        src.id,
		http_ip,
	        browser(http_ua),
		browser_id(http_ua)
	    )

	    src.free=src.free-1
	    mysql.query([[update traf set hit=hit+1 where ts=NOW()  and ((type ='source' and id = %d) or (type ='client' and id = %d))]],src.id,value.o.cid)
	    mysql.query([[update traf set count=count+1 where ts=NOW()  and type ='client' and id = %d]],value.o.cid)
	    print (" b ");
	end
	table.insert(value.c,value.o.cid)
    end

    value.o=nil
    mem[1]:set(key,json.encode(value),nextday)
    table.foreach(value.c,function (k,v) print (v.." ") end)
end

print("   wait\n\n")


