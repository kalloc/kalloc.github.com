package.path =  root .. "/include/?.lua;".. root .. "/include/?/init.lua;/usr/share/lua/5.1/?.lua;/usr/share/lua/5.1/?/init.lua;/usr/local/share/lua/5.1//?.lua;/usr/local/share/lua/5.1//?/init.lua;" 
package.cpath = root .. "/include/lib/?.so;/usr/local/lib/lua/5.1/?.so;/usr/lib/lua/5.1/?.so;/usr/local/lib/lua/5.1/loadall.so" 
if not _G["mysql"] then
    _G.mysql={};
end
require "inc";

engine_start();

function error404 () 
    print ('Status: 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 169\r\n\r\n<html>\r\n<head><title>404 Not Found</title></head>\r\n<body bgcolor="white">\r\n<center><h1>404 Not Found</h1></center>\r\n<hr><center>nginx/0.4.13</center>\r\n</body>\r\n</html>\r\n')
end

if magnet.ua == nil then magnet.ua={} end
if magnet.source == nil then magnet.source={} end

function check_cookie() 
    local cookie=cgilua.cookies.get("cookie2")
    if cookie ~= nil then
	if cookie == string.upper(sha1.digest(http_ua..http_ip..tostring(math.floor((magnet.time)/86400)))) then
	    return cookie
	elseif  cookie == string.upper(sha1.digest(http_ua..http_ip.."xooy")) then
	    return false
	end
    end	
    return nil
end



function browser(str)
    if str:find('Gecko/') then return 'ff' end	

    if str:find(' Opera ') or str:find('Opera/') then return 'opera' end

    if str:find('MSIE') then return 'ie' end	

    return 'other';


end

function browser_id() 
    local uaid
    local uaname=browser(http_ua)
    if uaname == 'ff' then
	return 1;
    elseif uaname == 'opera' then
	return 2;
    elseif uaname == 'ie' then
	return 3;
    end
    return 4
--[[    if magnet.ua[http_ua] then
	uaid=magnet.ua[http_ua].id
    else
	local uaname=browser(http_ua)
	local cur=mysql.query("SELECT uaID from useragent where name = '%s'",http_ua)
	if cur ~=nil and  cur:numrows() > 0  then
    	    row=cur:fetch({})
	    uaid=row[1]
	    cur:close()
	else
    	    cur=mysql.query("insert into useragent VALUES ('','%s')",http_ua)
    	    uaid=mysql.lastid()
	end
	magnet.ua[http_ua]={id=uaid,type=uaname}
    end
    return uaid
]]
    
end


function get_source(code) 
    local src=magnet.source[code]
    -- or src.free % 25 == 0 
    if magnet.time % 60 == 0 and math.floor((math.random()*100))%5 == 0 then
        mysql.query("insert delayed ignore into stat (ts,clientID,sourceID,ip,ua,uaID,quality)  select ts,clientID,sourceID,ip,ua,uaID,quality from stat_day")
	mysql.query("delete from stat_day where floor(ts/1000000) <= floor(SUBDATE(NOW(),+1)/1000000)") 
    end

    if src == nil or src.ua == nil  or  (src.time+60) < magnet.time  or 1==1 then
        cur=mysql.query("select id,total,count,ua_ie,ua_op,ua_ff,ua_ot from traf,source where type = 'source' and ts = NOW() and (count/(TIME_TO_SEC(CURTIME())/60)) <= total/1440 and groupID = id and loginID = 0x%s and hash = '%s' and status = 'enable'",QUERY.id,QUERY.hash)
	if cur ~=nil and  cur:numrows() > 0  then
	    row=cur:fetch({},"a")
	    src={id=row.id,free=row.total-row.count,time=magnet.time}
	    src.ua={
	        ie=(row.ua_ie == 'true' and true or nil),
	        opera=(row.ua_op == 'true' and true or nil),
	        ff=(row.ua_ff == 'true' and true or nil),
	        other=(row.ua_ot == 'true' and true or nil)
	    }
	    magnet.source[code]=src
	    cur:close()
	else
	    return nil
	end
    end

--    print ("Src: "..(src.time+60-magnet.time).."-"..(src.free%25).."\n")
    return src
end

function get_nextday(n) 
    return ((math.ceil((magnet.time)/86400)*86400)+(n*86400))+magnet.offset
end

