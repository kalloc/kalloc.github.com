cur=mysql.query("select * from useragent")
print ("Content-Type:text/plain\n\n");
if cur:numrows() > 0 then 
    row=cur:fetch({})
    while row do 
	print(string.format( "%d\t%s\t%s\n",row[1],browser(row[2]),row[2]))
	row=cur:fetch({});
    end
end
