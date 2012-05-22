if os.getenv("DOCUMENT_ROOT") then
    root = os.getenv("DOCUMENT_ROOT")
else
    root = os.getenv("PWD")
end
package.path =  root .. "/?.lua;" .. root .. "/include/?.lua;".. root .. "/include/?/init.lua;" .. "./?.lua;/usr/share/lua/5.1/?.lua;/usr/share/lua/5.1/?/init.lua;" 

require "inc";
require "json";

local output={};
engine_start()


if not Token:check() then
    print ("Status: 302 Moved Permanently\nLocation: /?error\n\n");
    return
end


show(templatizate("main.tpl",{title = "Chat",site = "LostPlanet",login=Token.data.login}))

get_post()



