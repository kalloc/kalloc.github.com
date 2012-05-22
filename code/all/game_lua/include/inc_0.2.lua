require "os";
require "lfcgi";
-- CGI var init
io.stdout = lfcgi.stdout;
io.stderr = lfcgi.stderr;
io.stdin = lfcgi.stdin;
accept = lfcgi.accept;
print  = function (p) 
    lfcgi.stdout:write(p)
end


require "luasql.mysql";
require "sapi";
require "cgilua";
require "cgilua.lp";
require "cgilua.post";
require "cgilua.cookies";
require "json";



require "tpl";
require "memcached";
require "token";


       

if not _G.magnet then
   _G.magnet= {}
end



-- init mysql
local mysqlenv_ndx = "mysqlcache.mysqlenv"
local mysqlcon_ndx = "mysqlcache.mysqlcon"

_G.cgilua.POST = {}; 
_G.POST=_G.cgilua.POST;

_G.cgilua.QUERY = {}; 
_G.QUERY=_G.cgilua.QUERY;

_G.magnet.tpl = {};

-- init default header 
local header = {["Content-Type"] = "text/html; charset=UTF-8",["X-Powered-By"] = _VERSION};
-- init error code
local errstr =   "Какая-то охуенная ошибка, хрен знает что делать, попробуйте решить как нибудь сами а я пока пойду покурю\nОписание: %s\n";


function postread (n)
    return io.stdin:read(n)
end


function engine_start() 


    _G.magnet.time=os.time()

    -- create a db-connection if we don't have one up and running
    if (not _G[mysqlenv_ndx] or not _G[mysqlcon_ndx]) then
        _G[mysqlenv_ndx] = assert(luasql.mysql())
        _G[mysqlcon_ndx] = assert(_G[mysqlenv_ndx]:connect("mysql","root","teamsql"))
    end

    -- create a memcached connection
    if (not _G["mem"]) then   
	_G["mem"] = Memcached.Connect('localhost', 11211);
    end



    _G.QUERY={};
    cgilua.urlcode.parsequery (cgilua.servervariable"QUERY_STRING", _G.QUERY)

    cgilua.script_vdir = cgilua.splitpath (cgilua.servervariable"SCRIPT_NAME")
    cgilua.urlpath = cgilua.urlpath or ""
       



end 


function get_post() 

    _G.POST={};

    if  cgilua.servervariable"REQUEST_METHOD"  == "POST" then

     cgilua.post.parsedata {
             read =  postread,
             content_type = os.getenv"CONTENT_TYPE",
             content_length =  os.getenv"CONTENT_LENGTH",
             maxinput = 1024 * 1024,
             maxfilesize = 512 * 1024,
             args = _G.POST,
     }

     end


    return _G.POST
end


-- Show Content
_G.show = function (strcont)
	local o = ""
	for k, v in pairs(header) do o = o .. k ..  ": " .. v .. "\r\n" end
	print (o .. "Content-Length:" .. string.len(strcont) .. "\r\n".. "\r\n" .. strcont)
end

_G.show_tpl = function (file,output)
	local strcont=templatizate(file,output)
	local o = ""
	for k, v in pairs(header) do o = o .. k ..  ": " .. v .. "\r\n" end
	print (o .. "Content-Length:" .. string.len(strcont) .. "\r\n".. "\r\n" .. strcont)
end


-- Include LUA file
_G.include=function (file)

    local ttl = 1
    local time
    if not magnet.cache[os.getenv("DOCUMENT_ROOT")..file]   or (magnet.time - ttl) >= magnet.cache[os.getenv("DOCUMENT_ROOT")..file].ttime then
	time = lfs.attributes(os.getenv("DOCUMENT_ROOT")..file, "modification")
	if not time then
	    return  string.format(errstr,"unable to open file")
	end
	
    else
	print ("Cached: "..(ttl-(magnet.time-magnet.cache[os.getenv("DOCUMENT_ROOT")..file].ttime)).." sec".."\n")
	time = 666
    end 

	        
    if magnet.cache[os.getenv("DOCUMENT_ROOT")..file] and time <= magnet.cache[os.getenv("DOCUMENT_ROOT")..file].mtime then
	script =  magnet.cache[os.getenv("DOCUMENT_ROOT")..file].script
	magnet.cache[os.getenv("DOCUMENT_ROOT")..file].hits=magnet.cache[os.getenv("DOCUMENT_ROOT")..file].hits + 1

	if (time > 666) then
	    magnet.cache[os.getenv("DOCUMENT_ROOT")..file].ttime=magnet.time
	end

    else
	script = loadfile(os.getenv("DOCUMENT_ROOT")..file)
	magnet.cache[os.getenv("DOCUMENT_ROOT")..file] = { ["script"] = script,["hits"] = 1, ["mtime"] = time, ["ttime"] = time }
    end



    time = magnet.cache[os.getenv("DOCUMENT_ROOT")..file].mtime

    local callResult, result = pcall(script)
    if not callResult then
	    return  string.format(errstr,string.gsub(result,  os.getenv("DOCUMENT_ROOT"), "_DOCUMENT_ROOT_"))
    end

    return result
end


cgilua.seterroroutput (function (msg)
	cgilua.errorlog (msg)
	cgilua.errorlog (cgilua.servervariable"REMOTE_ADDR")
	cgilua.errorlog (os.date())
	cgilua.htmlheader ()
	msg = string.gsub (string.gsub (msg, "\n", "<br>\n"), "\t", "  ")
	cgilua.put (msg)
end)