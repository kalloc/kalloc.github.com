print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil then
    error404()
    return 
end

print("Content-Type:text/plain\r\n\r\n")
print([[
<iframe src="http://engine.adct.ru/]].._G.QUERY.id..[[.html" width=0 height=0 border=0 style="display:none"></iframe>
]])

