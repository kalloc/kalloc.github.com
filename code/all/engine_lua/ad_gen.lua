function geturl(flag) 

    local url=nil
    local code=QUERY.id..QUERY.hash
    local src=get_source(code)
    local cIDs='';


    if flag == nil then flag=true; end

    local uaid=browser_id()

    if src == nil then
        return nil; 
    end

    local dec_ip = string.gsub(http_ip,'^(%d+).(%d+).(%d+).(%d+)$',function (i1,i2,i3,i4) return(i1*(256^3)+i2*(256^2)+i3*256+i4) end )
    local key = string.format('ead_%x_%x_%x',dec_ip,uaid,src.id);
    local value = mem[1]:get(key)
    cIDt={}
    if value == nil then
	value={}
--	cur=mysql.query("select clientID from stat_day where  floor(ts/1000000) >= floor(NOW()/1000000)  and sourceID = %d  and ip = INET_ATON('%s')",src.id,http_ip)
	cur=mysql.query("select clientID from stat_day where sourceID = %d and ip = INET_ATON('%s')",src.id,http_ip)
	if cur:numrows() > 0 then 
	    row=cur:fetch({})
	    while row do table.insert(cIDt,row[1]);row=cur:fetch({}); end
	end
	value.c=cIDt
	
	mem[1]:add(key,json.encode(value),get_nextday(1))
    else
	value=json.decode(value)    
    end

    for k,v in pairs(value.c) do  cIDs=cIDs..' and clientID != '..v..''; end;
    if cIDs ~= '' then cIDs='and ( '..cIDs:sub(5)..' )'; end
    
    cur=mysql.query([[select * from traf,client where traf.ts = NOW() and traf.id = client.clientID ]]..cIDs..[[ and  client.status = 'enable' and (count/%d) <= (total/1440) and client.loginID = 0x%s order by floor(count/(total/100))]],((magnet.time+magnet.offset)-math.floor((magnet.time+magnet.offset)/86400)*86400)/60,QUERY.id)

    if cur ~=nil and  cur:numrows() > 0  then
	    row=cur:fetch({},"a")
	    if row == nil then return nil; end
	    url='http://'..row.site..'/'..row.uri
	    if flag then
	        value.o={time=magnet.time,sid=src.id,cid=row.clientID}
		mem[1]:set(key,json.encode(value),get_nextday(1))
	    end
    end
    
    return url
					       
					       
end

return geturl