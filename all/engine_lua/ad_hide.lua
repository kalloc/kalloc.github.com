print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return
end

local output=[[<html><head><meta http-equiv="refresh" content="0; url=/]]..QUERY.id..QUERY.hash..[[_ref.html?]]..magnet.time..[["></head></html>]]

local c=check_cookie()
if c ~= nil then
    if c ~= false then
	url=include('ad_gen.lua')();
	if url then
	    output=[[<html><head><meta http-equiv="refresh" content="0; url=]]..url..[["></head></html>]]
	else
	    output=tostring(url)
	end
    else
	output=[[false]]
    end
end
	
		    

show(output)


