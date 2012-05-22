print ("Content-Type:text/html\r\n\r\n")

maxid=660
cur=mysql.query("SELECT MAX(groupID) as max from source")
   if cur ~=nil then
	   row=cur:fetch({})
           maxid=(row ~= nil) and row[1] or maxid
   end
						
maxid=maxid+1
print(maxid)
-- close everything
cur:close()

