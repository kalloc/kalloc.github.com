output={}
stat={}

if not Token:check() then
    errstr=nil
    if os.getenv("REQUEST_METHOD") == "POST" then
	get_post();
	if POST.login == nil or string.len(POST.login) == 0 or POST.login:match([[(['"]+)]]) ~= nil --'"
	    or
	   POST.password == nil or string.len(POST.password) == 0 or POST.password:match([[(['"]+)]]) ~= nil 
	then
	    errstr="Логин или пароль не указан"
	else
    	    cur=mysql.query("SELECT * from account where login = '%s' and pass = MD5('%s')",POST.login,POST.password)
	    if cur ==nil or cur:numrows() == 0 then
		errstr="Логин или пароль не верен";
	    else
		Token:start()
		row=cur:fetch({},"a")
		Token:save({login=row.login,id=row.loginID})
	        print ("Status: 302 Moved Permanently\nLocation: /admin/\n\n");
	        return
		    
	    end
	    
	end
    end

    show_tpl("admin_login.tpl",{title = "Admin :: Client :: Login",error=errstr})
    return
end
	

if QUERY.exit then
    Token:del()
    print ("Status: 302 Moved Permanently\nLocation: /admin/\n\n");
    return


end

if QUERY.client then

    --добавление новых данных
    if QUERY.add then
	errstr=nil
	output.title = "Admin ("..Token.data.login..") :: Client :: add"

	--есть POST парсим
	if os.getenv("REQUEST_METHOD") == "POST" then
	    get_post();
	    traf={}
	    site=""
	    uri=""

	    if POST.url == nil or POST.url:len() == 0  or POST.url:match([[(['"]+)]]) ~= nil then --'""
		errstr="url не указано или содержит в себе не допустимые символы"
	    else
		output.url=POST.url
		string.gsub(
		    string.gsub(POST.url,"http://",""),
		    "^([^/]+)/?(.*)$",
		    function (s,u) 
			site=s;
			uri=(u == nil or u == "/" or u == "") and "" or u;
		    end
		)
	    end

	    if POST.traf == nil or POST.traf:len() == 0 then
		errstr="трафик не указан"
	    end

	    if errstr == nil then 

		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]([0-9][0-9]) ([0-9]+)",
		    function (year,month,day,total) 
			data=os.time({year=year,month=month,day=day});
			if math.floor(data/86400) >= math.floor(magnet.time/86400) then
			    table.insert(traf,{data=data,total=total});
			end
		    end
		)

		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]XX ([0-9]+)",
		    function (year,month,total) 
			for day=1,31 do
			    data=os.time({year=year,month=month,day=day});
			    if math.floor(data/86400) >= math.floor(magnet.time/86400) then
				table.insert(traf,{data=data,total=total});
			    end
			end
		    end
		)


		if #traf == 0 then
		    errstr="трафик вводится по формату (гггг-мм-дд К), разделитель перенос строки"
		end
	    end
	    if errstr == nil then 
    		cur=mysql.query("SELECT * from client where uri = '%s' and site = '%s' and loginID = %d",uri,site,Token.data.id)
		if cur ~=nil  and cur:numrows() > 0 then
		    errstr="Такой url уже есть в базе, введите другой";
		end
	    end

	    if errstr == nil then 
		output.ok="URL - &laquo;<b>"..POST.url.."</b>&raquo;, добавлен "
		
		mysql.query("insert into client VALUES('',%d,'%s','%s','enable')",Token.data.id,uri,site)
		local newid=mysql.lastid()
		for k,v in pairs(traf) do
		    mysql.query("insert into traf VALUES(%d,FROM_UNIXTIME(%d),'client',%d,0,0,0)",newid,v.data,v.total*1000)
		end
	    end
        end
	output.error=errstr;
	--если данных нету выводим просто шаблон для ввода
	show_tpl("admin_client_add.tpl",output)

    else
	--добавление данных нету

	local status=nil
	if QUERY.id then

	    --неформат id
	    if QUERY.id:match('^[0-9]+$') == nil then
	        print ("Status: 302 Moved Permanently\nLocation: /admin/client/\n\n");
		return		
	    end

	    --смотрим клиента по его id
    	    cur=mysql.query("SELECT CONCAT('http://',site,'/',uri) as url,status from client where clientID =  %d and loginID = %d",QUERY.id,Token.data.id)

	    --если данных таких нету выкидываем в админку 
	    if cur ==nil  or cur:numrows() == 0 then
	        print ("Status: 302 Moved Permanently\nLocation: /admin/client/\n\n");
		return		
	    end
	    row=cur:fetch({})
	    output.url=row[1]
	    status=row[2]
	else
	
	    cur=mysql.query([[select clientID as id,CONCAT('http://',site,'/',uri) as url,sum(count) as hit from client,traf where clientID  = id and client.loginID = %d group by clientID order by clientID]],Token.data.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[row.id]={}
		    stat[row.id].all=row.hit
		    stat[row.id].name=row.url
        	    row=cur:fetch({},"a")
    		end
	    end

	    cur=mysql.query([[select clientID as id,CONCAT('http://',site,'/',uri) as url,sum(count) as hit,sum(total) as total from client,traf where clientID  = id and ts = NOW() and client.loginID = %d group by clientID order by clientID]],Token.data.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[row.id].today=row.hit
		    stat[row.id].total=row.total
        	    row=cur:fetch({},"a")
    		end
	    end

	    output.stat=stat
	
	end

	if QUERY.remove then
	    mysql.query([[delete from client where clientID = %d and loginID = %d]],QUERY.id,Token.data.id)
	    mysql.query([[delete from traf where type = 'client' and id = %d]],QUERY.id)
    	    print ("Status: 302 Moved Permanently\nLocation: /admin/client/\n\n");
	    return		
	
	end

	--есть добавление новых данных
	if os.getenv("REQUEST_METHOD") == "POST" and QUERY.id then
	    get_post();
	    traf={}

	    --меняем статус
	    if  POST.status  and POST.status == 'on' and status == 'disable' then
		    mysql.query([[update client set status = 'enable' where clientID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    status='enable'
	    elseif status == 'enable' and POST.status == nil then
		    mysql.query([[update client set status = 'disable' where clientID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    status='disable'
	    end


	    --ошибка какая-то, или пустой ввод
	    if POST.traf == nil or POST.traf:len() == 0 then
		errstr="трафик не указан"
	    else
		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]([0-9][0-9]) ([0-9]+)",
		    function (year,month,day,total) 
			data=os.time({year=year,month=month,day=day});
			if math.floor(data/86400) >= math.floor(magnet.time/86400) then
			    table.insert(traf,{data=data,total=total});
			end
		    end
		)
		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]XX ([0-9]+)",
		    function (year,month,total) 
			for day=1,31 do
			    data=os.time({year=year,month=month,day=day});
			    if math.floor(data/86400) >= math.floor(magnet.time/86400) then
				table.insert(traf,{data=data,total=total});
			    end
			end
		    end
		)
		
		
		if #traf == 0 then
		    errstr="вы не ввели ни одной подходящей по формату строки.<br/>трафик вводится по формату (гггг-мм-дд К), разделитель перенос строки"
		end
	    end

	    if errstr == nil then 
		output.ok="трафег изменен "
		for k,v in pairs(traf) do
		    mysql.query("insert into traf VALUES(%d,FROM_UNIXTIME(%d),'client',%d,0,0,0) ON DUPLICATE KEY UPDATE total=%d",QUERY.id,v.data,v.total*1000,v.total*1000)
		end
	    end
        end
	output.error=errstr;


	output.title = "Admin ("..Token.data.login..") :: Client"
	client=nil
	cur=mysql.query([[select clientID as id,CONCAT("http://",site,"/",uri) as url,site,uri from client where loginID = %d order by site,uri;]],Token.data.id)
	if cur ~=nil then
	    client = {}
	    row=cur:fetch({},"a")
    	    while row do
		if client[row.site] == nil then client[row.site]={} end
        	table.insert(client[row.site],row)
        	row=cur:fetch({},"a")
    	    end
	end
	output.client=(client ~= nil) and client or nil;

	if QUERY.id then

	    cur=mysql.query([[select ts,total,count from traf,client where traf.id = client.clientID and client.loginID = %d and  id = %d and type = 'client' and ts >= NOW()]],Token.data.id,QUERY.id)
	    if cur ~=nil then
		traf = ""
		row=cur:fetch({},"a")
    		while row do
		    traf=traf..row.ts.." "..(row.total/1000).."\n"
        	    row=cur:fetch({},"a")
    		end
		output.traf=traf;
	    end

	    stat={}

	    local day=tonumber(mysql.one([[select SUBSTR(LAST_DAY(NOW()),'-2')]]))
	    for d=1,day do stat[d]={hours={},t=0}; for h=1,8 do stat[d].hours[h]=0; end; end	    


	    cur=mysql.query([[select UNIX_TIMESTAMP(ts) as time,count(*) as num from stat where floor(ts/1000000) >= floor(SUBDATE(NOW(),DAYOFMONTH(NOW())-1)/1000000) and ts <= LAST_DAY(NOW()) and clientID = %d group by FLOOR(ts/10000);]],QUERY.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[d.day].t=stat[d.day].t+row.num
		    stat[d.day].hours[math.floor(d.hour/3)+1]=stat[d.day].hours[math.floor(d.hour/3)+1]+row.num
        	    row=cur:fetch({},"a")
    		end
	    end
	    output.day=os.date('%Y-%m-xx')
	    output.today=tonumber(os.date('%d'))
	    output.stat=stat
	    output.id=QUERY.id
	    if status == 'enable' then output.checked=true end


	end

	show_tpl("admin_client.tpl",output)
	
				
    end

elseif  QUERY.source  then

    errstr=nil

    if QUERY.add then
	output.title = "Admin ("..Token.data.login..") :: source :: add"
	if os.getenv("REQUEST_METHOD") == "POST" then
	    get_post();

	    if POST.name == nil or POST.name:len() == 0  or POST.name:match([[(['"]+)]]) ~= nil then --'""
		errstr="имя не указано или содержит в себе не допустимые символы"
	    else
		output.name=POST.name
	    end

	    if POST.count == nil or POST.count:match("^([%d]+)$") == nil or POST.count == 0  then
		errstr="количество должно быть  являться цифрой и быть больше нуля"
	    end
	    if errstr == nil then 
    		cur=mysql.query("SELECT * from source where loginID = '%d' and name = '%s'",Token.data.id,POST.name)
		if cur ~=nil  and cur:numrows() > 0 then
		    errstr="Такое имя уже есть в базе, выберите другое";
		end
	    end

	    if errstr == nil then 
		maxid=665
    		cur=mysql.query("SELECT MAX(groupID) from source")
		if cur ~=nil then
		    row=cur:fetch({}) 
		    maxid=(row ~= nil) and row[1] or maxid
		end
		maxid=maxid+1

		output.ok="источник - &laquo;<b>"..POST.name.."</b>&raquo;, добавлен "
	

		for i=1,POST.count do
		    newid=string.upper(string.gsub(uuid.new("r"),"-",""))
    		    mysql.query("insert into source VALUES('',%d,%d,'%s','%s','enable','%s','%s','%s','%s')",maxid,Token.data.id,POST.name,newid,POST.ua_ie and 'true' or 'false',POST.ua_op and 'true' or 'false',POST.ua_ff and 'true' or 'false',POST.ua_ot and 'true' or 'false')
		end
	    end
        end
	output.error=errstr;

	show_tpl("admin_source_add.tpl",output)

    else

	local onerow
	if QUERY.id then
	    if QUERY.id:match('^[0-9]+$') == nil then
	        print ("Status: 302 Moved Permanently\nLocation: /admin/source/\n\n");
		return		
	    end

    	    onerow=mysql.onerow("SELECT * from source where groupID =  %d and loginID = %d group by groupID",QUERY.id,Token.data.id)

	    if onerow == nil then
	        print ("Status: 302 Moved Permanently\nLocation: /admin/source/\n\n");
		return		
	    end
	else
	
	    cur=mysql.query([[select groupID as id,name,sum(count) as count,total from source,traf where groupID  = id  and loginID = %d group by groupID desc;]],Token.data.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[row.id]={}
		    stat[row.id].total=row.total
		    stat[row.id].all=row.count
		    stat[row.id].name=row.name
        	    row=cur:fetch({},"a")
    		end
	    end

	    cur=mysql.query([[select groupID as id,name,sum(count) as count,sum(fail) as fail,sum(hit) as hit,total from source,traf where ts = NOW() and groupID  = id and loginID = %d group by groupID desc;]],Token.data.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[row.id].today=row.count
		    stat[row.id].hit=row.hit
		    stat[row.id].fail=row.fail
        	    row=cur:fetch({},"a")
    		end
	    end

	    output.stat=stat

	end


	if os.getenv("REQUEST_METHOD") == "POST" and QUERY.id then
	    get_post();
	    traf={}

	    if POST.traf == nil or POST.traf:len() == 0 then
		errstr="трафик не указан"
	    end
	    
	    if  POST.status  and POST.status == 'on' and onerow.status == 'disable' then
		    mysql.query([[update source set status = 'enable' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.status='enable'
	    elseif onerow.status == 'enable' and POST.status == nil then
		    mysql.query([[update source set status = 'disable' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.status='disable'
	    end


	    if  POST.ua_ie  and POST.ua_ie == 'on' and onerow.ua_ie == 'false' then
		    mysql.query([[update source set ua_ie = 'true' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ie='true'
	    elseif onerow.ua_ie == 'true' and POST.ua_ie == nil then
		    mysql.query([[update source set ua_ie = 'false' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ie='false'
	    end


	    if  POST.ua_op  and POST.ua_op == 'on' and onerow.ua_op == 'false' then
		    mysql.query([[update source set ua_op = 'true' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_op='true'
	    elseif onerow.ua_op == 'true' and POST.ua_op == nil then
		    mysql.query([[update source set ua_op = 'false' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_op='false'
	    end

	    if  POST.ua_ff  and POST.ua_ff == 'on' and onerow.ua_ff == 'false' then
		    mysql.query([[update source set ua_ff = 'true' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ff='true'
	    elseif onerow.ua_ff == 'true' and POST.ua_ff == nil then
		    mysql.query([[update source set ua_ff = 'false' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ff='false'
	    end


	    if  POST.ua_ot  and POST.ua_ot == 'on' and onerow.ua_ot == 'false' then
		    mysql.query([[update source set ua_ot = 'true' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ot='true'
	    elseif onerow.ua_ot == 'true' and POST.ua_ot == nil then
		    mysql.query([[update source set ua_ot = 'false' where groupID = %d and loginID = %d]],QUERY.id,Token.data.id)
		    onerow.ua_ot='false'
	    end


	    if errstr == nil then 
		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]([0-9][0-9]) ([0-9]+)",
		    function (year,month,day,total) 
			data=os.time({year=year,month=month,day=day});
			if math.floor(data/86400) >= math.floor(magnet.time/86400) then
			    table.insert(traf,{data=data,total=total});
			end
		    end
		)

		POST.traf:gsub(
		    "([0-9][0-9][0-9][0-9])[-]([0-9][0-9])[-]XX ([0-9]+)",
		    function (year,month,total) 
			for day=1,31 do
			    data=os.time({year=year,month=month,day=day});
			    if math.floor(data/86400) >= math.floor(magnet.time/86400) then
				table.insert(traf,{data=data,total=total});
			    end
			end
		    end
		)

		if #traf == 0 then
		    errstr="трафик вводится по формату (гггг-мм-дд К), разделитель перенос строки"
		end
	    end

	    if POST.count and tonumber(POST.count) > 0 then
    	        local name=mysql.one("SELECT name from source where groupID =  %d and loginID = %d group by groupID",QUERY.id,Token.data.id)

		for i=1,POST.count do
		    newid=string.upper(string.gsub(uuid.new("r"),"-",""))
    		    mysql.query("insert into source VALUES('',%d,%d,'%s','%s','%s','%s','%s','%s','%s')",QUERY.id,Token.data.id,name,newid,onerow.status,onerow.ua_ie,onerow.ua_op,onerow.ua_ff,onerow.ua_ot)
		end
		output.ok="ссылка(и) добавлены ("..POST.count.."). "
	    end

	    if errstr == nil then 
		
		output.ok=(output.ok or "").."трафег изменен "
		for k,v in pairs(traf) do
		    mysql.query("insert into traf VALUES(%d,FROM_UNIXTIME(%d),'source',%d,0,0,0) ON DUPLICATE KEY UPDATE total=%d",QUERY.id,v.data,v.total*1000,v.total*1000)
		end
	    end
        end
	output.error=errstr;





	cur=mysql.query("SELECT groupID as id,name,count(groupID) as count from source where loginID = %d  group by groupID",Token.data.id)
	source={}
	if cur ~=nil then
	    row=cur:fetch({},"a")
    	    while row do
        	table.insert(source,row)
        	row=cur:fetch({},"a")
    	    end
	end

	output.source=(#source > 0) and source or nil;
	output.title="Admin ("..Token.data.login..") :: source"


	if QUERY.id  then
	    if QUERY.del and  QUERY.del:match('^[0-9]+$') then
		mysql.query([[delete from source where sourceID = %d and groupID ]],QUERY.del,QUERY.id)
		mysql.query([[delete from traf where type = 'source' and id = %d]],QUERY.del)
	        print ("Status: 302 Moved Permanently\nLocation: /admin/source/?id="..QUERY.id.."\n\n");
		return		
	    end

	    cur=mysql.query([[select ts,total,count from traf where id = %d and ts >= NOW() and type = 'source']],QUERY.id)
	    if cur ~=nil then
		traf = ""
		row=cur:fetch({},"a")
    		while row do
		    traf=traf..row.ts.." "..(row.total/1000).."\n"
        	    row=cur:fetch({},"a")
    		end
		output.traf=traf;
	    end

	    cur=mysql.query([[select hash,sourceID as id,name from source where groupID = %d and loginID = %d;]],QUERY.id,Token.data.id)
	    local count_traf=0
	    if cur ~=nil then
		hash = {}
		row=cur:fetch({},"a")
    		while row do
		    count_traf=count_traf+1
		    output.name=row.name

		    table.insert(hash,{hash=string.format("%0.6X%s",Token.data.id,row.hash),id=row.id})
        	    row=cur:fetch({},"a")
    		end
		output.hash=hash;
		output.count=count_traf
	    end

	    stat={}

	    local day=tonumber(mysql.one([[select SUBSTR(LAST_DAY(NOW()),'-2')]]))
	    for d=1,day do stat[d]={hours={},t=0}; for h=1,8 do stat[d].hours[h]=0; end; end	    


	    cur=mysql.query([[select UNIX_TIMESTAMP(ts) as time,count(*) as num from stat where floor(ts/1000000) >= floor(SUBDATE(NOW(),DAYOFMONTH(NOW())-1)/1000000) and ts <= LAST_DAY(NOW()) and sourceID = %d and quality = 'true' group by FLOOR(ts/10000);]],QUERY.id)
	    if cur ~=nil then
		row=cur:fetch({},"a")
    		while row do
		    local d=os.date('*t',row.time)
		    stat[d.day].t=stat[d.day].t+row.num
		    stat[d.day].hours[math.floor(d.hour/3)+1]=stat[d.day].hours[math.floor(d.hour/3)+1]+row.num
        	    row=cur:fetch({},"a")
    		end
	    end
	    output.day=os.date('%Y-%m-xx')
	    output.stat=stat
	    output.today=tonumber(os.date('%d'))

	end
	
	if onerow and  onerow.status == 'enable' then output.checked=true end
	if onerow and  onerow.ua_ie == 'true' then output.ie_checked=true end
	if onerow and  onerow.ua_op == 'true' then output.op_checked=true end
	if onerow and  onerow.ua_ff == 'true' then output.ff_checked=true end
	if onerow and  onerow.ua_ot == 'true' then output.ot_checked=true end
	output.id=QUERY.id;
	show_tpl("admin_source.tpl",output)
    end

else

    show_tpl("admin.tpl",{title = "Admin ("..Token.data.login..") :: Main",site = "AD.CT"})

end
