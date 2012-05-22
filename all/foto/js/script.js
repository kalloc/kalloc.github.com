
function include(script_filename) {
    var html_doc = document.getElementsByTagName('head').item(0);
    var js = document.createElement('script');
    js.setAttribute('language', 'javascript');
    js.setAttribute('type', 'text/javascript');
    js.setAttribute('src', script_filename);
    html_doc.appendChild(js);
}


if (window.innerWidth)
{
	docwidth = window.innerWidth;
	docheight= window.innerHeight;
}
else if (document.documentElement && document.documentElement.clientWidth)
{
	docwidth= document.documentElement.clientWidth;
	docheight = document.documentElement.clientHeight;
}
else if (document.body)
{
	docwidth= document.body.clientWidth;
	docheight = document.body.clientHeight;
}
else if (screen.width)
{
	docwidth = screen.width;
	docheight= screen.height;
	
}


var BASEURI = document.location.pathname;
if(BASEURI.indexOf("/",1) > 0) { BASEURI  = BASEURI.substring(0, BASEURI.indexOf("/",1))+"/"; }
else if(BASEURI.indexOf("/",1) == 0) { BASEURI = ""; }

include('/js/func.js');
include('/js/string.js');
include('/js/jquery.js');
include('/js/imagebox.js');
include('/js/popuplayer.js');
include('/js/ajax/ajax.js');
