print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return 
end

local output=""
local c=check_cookie()

if c ~= nil and c ~= false then
    url=include('ad_gen.lua')(false);
    if url ~= nil then
	output=[[true]]
    else
        output=[[wait]]
    end
else
    output=[[false]]
end	




show(output)

