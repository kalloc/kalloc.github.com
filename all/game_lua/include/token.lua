require "sha1";
require "uuid";

Token = {
    id=nil,
    uid=nil,
    auth=false,
    data={},
    start = function()
	Token.data={}
	if not Token.check() then
	    Token.auth=false
	    cgilua.cookies.set("uuid",Token.new());
	    if not (cgilua.servervariable"HTTP_X_REQUEST") or not cgilua.servervariable"HTTP_X_REQUEST" == "JSON"then
		print ("Status: 302 Moved Permanently\nLocation: "..cgilua.servervariable"REQUEST_URI".."\n\n");
		return nil
	    else
		return 1
	    end
	else 
	    Token.auth=true
	    return Token.id
	end
    end,
    del = function()
        cgilua.cookies.delete("uuid");
    end,
    check = function() 
	Token.id=cgilua.cookies.get("uuid")
	if Token.id == 1  or not Token.id then
	    return false
	end

	Token.uid=string.sub(Token.id,0,32)
	if not string.match(Token.uid,"^(%x+)$") then
	    return false
	end

	if (string.upper(sha1.digest(Token.uid..cgilua.servervariable"HTTP_USER_AGENT"))) ~= string.sub(Token.id,33) then
	    return false
	end
	local s = getmem(Token.uid):get("sess_"..Token.uid)
	if s == nil or s == "" then
	    Token.del()
	    return false
	end
	
	Token.data=json.decode(s)
	return true
    end,
    merge = function(Token,p,s) 
	if type(s) ~= "table" then
	    return p
	end
	for k,v in pairs(s) do
	    if type(v) == "table" and type(p[k]) == "table" then
		Token.merge(p[k],s[k])	    
	    else
		p[k] = v
	    end
	end
	return p
    end,
    save = function(Token,t) 
	Token.data=Token.merge(Token.data,t)
	getmem(Token.uid):add("sess_"..Token.uid,json.encode(Token.data))
    end,
    new = function ()
	Token.uid=string.upper(string.gsub(uuid.new("random"),"-",""))
	Token.id=string.upper(Token.uid..sha1.digest(Token.uid..cgilua.servervariable"HTTP_USER_AGENT"))
	return Token.id
    end

}

