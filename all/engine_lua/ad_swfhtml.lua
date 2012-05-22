print("Cache-Control: max-age=0\r\nCache-Control: no-cache\r\n")

if QUERY.id == nil then
    error404()
    return 
end

print("Content-Type:text/html\r\n\r\n")
print([[
<html>
<head></head>
<body>
<script>
try {
    axo = new ActiveXObject("ShockwaveFlash.ShockwaveFlash");
    version = axo.GetVariable("$version");
    location.href="http://engine.adct.ru/]]..QUERY.id..[[.swf"+"?"+(Math.random()*666)
} catch(e) {

}
</script>
<!--
<object width="100%" height="100%" codetype="application/x-shockwave-flash">
<param value="http://engine.adct.ru/]].._G.QUERY.id..[[.swf?]]..magnet.time..[[" name="movie"/>
<param name="autostart" value="true" />
<param name="controller" value="true" />
</object>
-->


</body>
</html>
]])

