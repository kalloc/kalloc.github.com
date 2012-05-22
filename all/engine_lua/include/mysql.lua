require "luasql.mysql";

-- init mysql
local mysqlenv_ndx = "env"
local mysqlcon_ndx = "con"


-- create a db-connection if we don't have one up and running
function init_mysql() 
    _G.mysql[mysqlcon_ndx] = assert(_G.mysql[mysqlenv_ndx]:connect("adct","adct","adct","localhost"))
end

if (not _G.mysql[mysqlenv_ndx] or not _G.mysql[mysqlcon_ndx]) then
    _G.mysql[mysqlenv_ndx] = assert(luasql.mysql())
    init_mysql() 
end

_G.mysql["query"] = function (...)
    local str=string.format(...)
    local cur,err=_G.mysql[mysqlcon_ndx]:execute(str)
    if err  ~= nil then
        init_mysql()
	cur=_G.mysql[mysqlcon_ndx]:execute(str)
    end
    return cur
end

_G.mysql["lastid"] = function ()
    cur=_G.mysql.query("SELECT LAST_INSERT_ID()")
    if cur == nil then
	return nil
    end

    row=cur:fetch({})

    if row == nil then
	return nil
    end
    
    return row[1]
end

_G.mysql["one"] = function (...)
    cur=_G.mysql.query(...)
    if cur == nil then
	return nil
    end

    row=cur:fetch({})

    if row == nil then
	return nil
    end
    
    return row[1]
end


_G.mysql["onerow"] = function (...)
    cur=_G.mysql.query(...)
    if cur == nil then
	return nil
    end

    row=cur:fetch({},"a")

    if row == nil then
	return nil
    end
    
    return row
end
