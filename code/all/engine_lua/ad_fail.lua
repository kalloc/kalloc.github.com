print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil or QUERY.hash == nil then
    error404()
    return 
end

local id=mysql.one([[select groupID from source where source.hash = '%s' and loginID = 0x%s]],QUERY.hash,QUERY.id)
if id then
    mysql.query([[update traf set fail=fail+1 where ts=NOW()  and (type ='source' and id = %d)]],id)
end



