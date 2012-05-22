print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return 
end

local output=""
local c=check_cookie();
if c ~= nil then
    output=[[false]]

    if c ~= false then
	url=include('ad_gen.lua')();
	output=url
	if url ~= nil then 
	    print ("Status: 302 Moved Permanently\nLocation: "..url.."\n\n");
            return
	end
    end
else


    local ref=env("HTTP_REFERER") 

    if ref == nil then
	cgilua.cookies.set("cookie2",string.upper(sha1.digest(http_ua..http_ip..tostring(math.floor(magnet.time/86400)))))
    elseif ref:match("_ref") or QUERY.check then
	cgilua.cookies.set("cookie2",string.upper(sha1.digest(http_ua..http_ip.."xooy")))
	local id=mysql.one([[select groupID from source where source.hash = '%s']],QUERY.hash)
	if id then
	    mysql.query([[update traf set fail=fail+1 where ts=NOW()  and (type ='source' and id = %d)]],id)
	end
	print ("Status: 302 Moved Permanently\nLocation: /"..QUERY.id..QUERY.hash.."_fail\n\n");
	return
    else
	print ("Status: 302 Moved Permanently\nLocation: http://"..env("HTTP_HOST").."/"..QUERY.id..QUERY.hash.."_ref.html?check\n\n");
    end
end
show(output or "")


