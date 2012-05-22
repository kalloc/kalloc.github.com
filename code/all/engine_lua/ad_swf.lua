print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return 
end

local c=check_cookie()
local output=""
if c ~= nil then
    if c ~= false then
	url=include('ad_gen.lua')();
	if url == nil then
	    output=[[valid=0]]
	else 
	    output=[[valid=1&url=]]..url
	end
    else
	output=[[false]]
    end	
else 
    output=[[valid=1]]
end
show(output)


