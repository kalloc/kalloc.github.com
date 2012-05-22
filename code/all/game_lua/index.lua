if os.getenv("DOCUMENT_ROOT") then
    root = os.getenv("DOCUMENT_ROOT")
else
    root = os.getenv("PWD")
end
package.path =  root .. "/?.lua;" .. root .. "/include/?.lua;".. root .. "/include/?/init.lua;" .. "./?.lua;/usr/share/lua/5.1/?.lua;/usr/share/lua/5.1/?/init.lua;" 

require "inc";
local output={};
engine_start()

if Token:check() then
    print ("Status: 302 Moved Permanently\nLocation: /main.lua\n\n");
    return
end

                        



if  cgilua.servervariable"HTTP_X_REQUEST" == nil or cgilua.servervariable"HTTP_X_REQUEST" ~= "JSON"then
    show_tpl("index.tpl",{title = "Main",site = "LostPlanet",js='login'})
    return
end 

get_post()

if _G.POST['login'] and string.len(_G.POST['login']) > 0 then
    Token:start()
    local login=tostring(_G.POST['login'])
    Token:save({login=login})
--    cgilua.cookies.set("login",login);
    output={js=[[document.location='/main.lua']]}
else
    output={site = "LostPlanet",zzz=tostring(Token:check()),title = "Main",js=[[$('login_error').fade('show')]]}
end

show(json.encode(output))
