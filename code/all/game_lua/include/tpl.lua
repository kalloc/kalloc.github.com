

function templatizate(file,arg) 

    local ttl = 1
    local time
    local env = {}
    if os.getenv("DOCUMENT_ROOT") then
	file = os.getenv("DOCUMENT_ROOT").."/tpl/"..file
    else
	file = "./tpl/"..file
    end
    
    local expandvar = function ( var ) 
	if var and arg[var] then return arg[var] else return ""  end
    end
    setmetatable(env, {__index = {expandvar=expandvar,arg=arg}})


    if not magnet.tpl[file]  or (magnet.time - ttl) >= magnet.tpl[file].ttime then

        time = lfs.attributes(file, "modification")
        if not time then
            return  string.format("%s","unable to open tpl file")
        end

    else
        time = 666
    end


    if magnet.tpl[file] and time <= magnet.tpl[file].mtime then
        script =  magnet.tpl[file].script
        magnet.tpl[file].hits=magnet.tpl[file].hits + 1

        if (time > 666) then
            magnet.tpl[file].ttime=magnet.time
        end

    else
        local f = assert(io.open(file, "rb"))
        finded = "return [[".. string.gsub(f:read("*all"), "{{(%w+)}}",function (n)
		return "]] .. expandvar(\""..n.."\").. [["
	end) .."]]"
	script = loadstring(finded);
	if script == "nil" then 
	    return ""
	end

        magnet.tpl[file] = { ["script"] = script,["hits"] = 1, ["mtime"] = time, ["ttime"] = time }
    end



    setfenv(script,env)


    local callResult, result = pcall(script)
    if not callResult then
            return  string.format(errstr,string.gsub(result,  os.getenv("DOCUMENT_ROOT"), "_DOCUMENT_ROOT_"))
    end

    return result



end

