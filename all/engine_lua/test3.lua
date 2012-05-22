print ("Content-Type:text/html\r\n\r\n")



cur=assert(mysql.query("SELECT name,count(groupID) as count from source where loginID = %d group by groupID",Token.data.id))
local source={}
if cur ~=nil then
    row=cur:fetch({},"a")
    while row do
        table.insert(source,row)
        row=cur:fetch({},"a")
    end
end

table.foreach(source,function(k,v) print (tostring(v).." "..tostring(v.name).."=>"..tostring(v.count).."\n") end)
-- close everything
cur:close()
