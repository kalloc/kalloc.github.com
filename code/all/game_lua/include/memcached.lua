require("socket")

Memcached = {
    list = {
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211},
	{host="localhost",port=11211}
    },
--storage procedure
    set = function(cache, key, value,expire)
	return _store(cache, 'set', key, value,expire)
    end,

    add = function(cache, key, value,expire)
	return _store(cache, 'add', key, value,expire)
    end,

    replace = function(cache, key, value)
	return _store(cache, 'replace', key, value,expire)
    end,

    append = function(cache, key, value)
	return _store(cache, 'append', key, value,expire)
    end,

    prepend = function(cache, key, value)
	return _store(cache, 'prepend', key, value,expire)
    end,

--retrieval procedure
    get = function(cache, key)
	return _retrieve(cache, 'get ' .. key)
    end,

    stats = function(cache, key)
	if key == nil then
	    return _retrieve(cache, 'stats')
	end
        return _retrieve(cache, 'stats ' .. key)
    end,
--deletion procedure

    delete = function(cache, key)
	local res = _send(cache, 'delete ' .. key)

	if res == 'NOT_FOUND' then
	    return nil
	end

	if res ~= 'DELETED' then
	    error("Error deleting '" .. key .. "': " .. res)
	    return nil
	end

	return true
    end,

    incr = function(cache, key, val)
	if val == nil then 
	    val = 1
	end
	
	local res = _send(cache, 'incr ' .. key .. ' ' .. val)

	if res == 'ERROR' or res == 'CLIENT_ERROR' then
	    error("Error incrementing '" .. key .. "': " .. res)
	end

	return res
    end,

    decr = function(cache, key, val)
	if val == nil then 
	    val = 1
	end
	
	local res = _send(cache, 'decr ' .. key .. ' ' .. val)

	if res == 'ERROR' or res == 'CLIENT_ERROR' then
	    error("Error incrementing '" .. key .. "': " .. res)
	end

	return res
    end,

    Connect = function(address, port,proto)
	

	local client

	function open(host,port,proto) 
	    if proto == nil or proto ~= "udp" then
		client = socket.connect(address, port);
	    else 
		client = socket.udp();
		client:setpeername(address, port);
	    end
	    return client
	end

	if address == nil then
	    error('No host address defined')
	end


	if port == nil then
	    port = 11211
	end

	client=open(address,port,proto)
	
	if not client then
	    error('Could not connect to ' .. address .. ':' .. port)
	end

	function _send(cache, str)
	    local socket = cache.socket

	    socket:send(str .. "\r\n")
	    local line, err = socket:receive()
	    if err then
		if err:sub(-6) == "closed" then cache.socket=open(address,port,proto) end
	        error("Error send '" .. str .. "': " .. err)
	        return nil
	    end
	    return line
	end

	function _store(cache, op, key, value,expire)
	    if expire == nil then
		expire = 0
	    end
	    
	    local len = string.len(value)
	    local cmd = op .. ' ' .. key .. ' 0 '..expire..' ' .. len .. '\n' .. value

	    local res = _send(cache, cmd)

	    if res ~= 'STORED' then
		if err:sub(-6) == "closed" then cache.socket=open(address,port,proto) end
		error("Error storing '" .. key .. "': " .. res)
		return nil
	    end

	    return true
	end

	function _retrieve(cache, str)
	    local socket = cache.socket

	    socket:send(str .. '\n')

	    local data = {}
	    while true do
		local line, err = socket:receive()
		if err then
		    if err:sub(-6) == "closed" then cache.socket=open(address,port,proto) end
		    error("Error retrieve '" .. str .. "': " .. err)
		    return nil
		end
		
		

		if line == 'END' then
		    break
		elseif string.sub(line, 1, 5) == 'VALUE' then
		else
		    table.insert(data, line)
		end
	    end

	    if table.getn(data) == 0 then 
		return nil
	    end

	    local datastring = table.concat(data, '\n')

	    return datastring
	end


	local cache = {
	    socket = client,
	    set = Memcached.set,
	    stats = Memcached.stats,
	    add = Memcached.add,
	    replace = Memcached.replace,
	    get = Memcached.get,
	    delete = Memcached.delete,
	    incr = Memcached.incr,
	    decr = Memcached.decr,
	}

	return cache
    end
}

-- 
-- Memcached.lua
-- 
-- A pure lua implementation of a simple memcached client. Only 1 memcached server is currently supported. Requires the luasocket library.
-- See http://www.danga.com/memcached/ for more information about memcached.
--
--
--
-- Synopsis
--
-- require('Memcached')
--
-- memcache = Memcached.Connect('some.host.com', 11000)
-- 
-- memcache:set('some_key', 1234)
-- memcache:add('new_key', 'add new value')
-- memcache:replace('existing_key', 'replace old value')
--
-- cached_data = memcache:get('some_key')
--
-- memcache:delete('old_key')
--
--
--
-- Methods:
--
-- memcache = Memcached.Connect(host[, port])
--    Connect to memcached server at 'host' on port number 'port'. If port is not provider, port 11211 is used.
--    
-- memcache:set(key, value)
--    Unconditionally sets a key to a given value in the memcache.
--     
-- memcache:add(key, value)
--    Like set, but only stores in memcache if the key doesn't already exist.
--    
-- memcache:replace(key, value)
--    Like set, but only stores in memcache if the key already exists. The opposite of add.
--    
-- value = memcache:get(key)
--    Retrieves a key from the memcache. Returns the value or nil
--    
-- memcache:delete(key)
--    Deletes a key. Returns true on deletion, nil if the key was not found.
--    
-- value = memcache:incr(key[, value])
--   Sends a command to the server to atomically increment the value for key by value, or by 1 if value is nil. 
--   Returns nil if key doesn't exist on server, otherwise it returns the new value after incrementing. Value should be zero or greater.
--    
-- value = memcache:decr(key[, value])
--   Like incr, but decrements. Unlike incr, underflow is checked and new values are capped at 0. If server value is 1, a decrement of 2 returns 0, not -1.
--

    -- create a memcached connection
if (not _G["mem"]) then
    _G["mem"]={}
    table.foreach(Memcached.list,function(k,v) _G.mem[k] = Memcached.Connect(v.host, v.port) end)
end

function getmem(str) 
    return _G.mem[str:byte()%10]
end