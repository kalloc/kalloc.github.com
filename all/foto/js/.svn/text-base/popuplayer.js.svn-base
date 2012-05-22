/**
 * @author Andreas
 */
document.onselectstart = function() {return false;} // ie
//document.onmousedown = function() {return false;} // mozilla


var __style = new Array();

      
// Configuration
var _BG_Img_Resizable = "/js/transparent_1x1.gif";


// Registration style
__style["Registration"] = new Array();
__style["Registration"]["BorderColor"] = "#D8CEC4";
__style["Registration"]["CloseImage"] =  "/js/close_14x14.gif";


// Configuration End



var PopUp = new Array();

// PopUp Layer
function PopUp_Layer(_id,typ,tittle,src,x,y,w,h,_style){
	showFadeScreen();
	if(Array_exists(PopUp[_id])==false){PopUp[_id] = new Array();}
	PopUp[_id]["x"] = x;
	PopUp[_id]["y"] = y;
	PopUp[_id]["w"] = w;
	PopUp[_id]["h"] = h;
// 01. create MainLayer
	lib_create_obj_0001("div",(_id+"M_L"),"body","",500,"absolute","#FFFFFF","",("1px solid " + __style[_style]["BorderColor"]),"visible",(x+"px"),(y+1+"px"),(w+"px"),(h-2+"px"),"","","","","","","default","","","1px","","",10);
// 02. create Title Bar
	lib_create_obj_0001("div",(_id+"T_B"),(_id+"M_L"),"",500,"absolute","Transparent","","0px none Transparent","visible",(0+"px"),(0-2+"px"),(w+"px"),(20+"px"),"dragStart(event, this.parentNode.id);","","","","","","move","","","1px","","",10);
// 03. create inner window
	lib_create_obj_0001("div",(_id+"I_W"),(_id+"M_L"),"",500,"absolute","#FFFFFF","","0px none #FFFFFF","visible",(0+"px"),(20-2+"px"),(w+"px"),(h-20+"px"),"","","","","","","default","","","","","",10);
// 04. create iframe
	if("iframe"==typ.toLowerCase()){lib_create_obj_0001("iframe",(_id+"I_F"),(_id+"I_W"),"",500,"absolute","#FFFFFF","","","visible",(0+"px"),(0+"px"),(w+"px"),(h-20+"px"),"","","",src,"auto","0","default","","","","","block",10);}
// 05. innerHTML
	if("div"==typ.toLowerCase()){document.getElementById((_id+"I_W")).innerHTML = document.getElementById(src).innerHTML}

// 14. create tittle_text_layer
	lib_create_obj_0001("div",(_id+"TT_L"),(_id+"T_B"),"",500,"absolute","Transparent","","0px none Transparent","visible",(4+"px"),(3+"px"),(160+"px"),(11+"px"),"","",tittle,"","","","","Arial, Helvetica, sans-serif","bold","11px","#FFFFFF","inline",10);
// 15. create close_button_layer
	lib_create_obj_0001("div",(_id+"CB_L"),(_id+"M_L"),"",500,"absolute","Transparent",(__style[_style]['CloseImage']),"0px none Transparent","visible",(w-20+"px"),(0+"px"),(18+"px"),(18+"px"),"","var S01=this.parentNode.id;var S02=S01.indexOf(\"M_L\",0);S01=S01.substring(0,S02);PopUp_Layer_Close(S01);","","","","","","","","","","",10);
// 16. create close_text_layer
	lib_create_obj_0001("div",(_id+"CL_L"),(_id+"M_L"),"",500,"absolute","Transparent","","","visible",((w/2)-10+"px"),(h-30+"px"),(100+"px"),(40+"px"),"","var S01=this.parentNode.id;var S02=S01.indexOf(\"M_L\",0);S01=S01.substring(0,S02);PopUp_Layer_Close(S01);","<a href=\"#\">close</a>","","","","hand","Arial, Helvetica, sans-serif","bold","11px","black","inline",10);

}// PopUp Layer End


// (true|false)Array_Exists(Array_var)
function Array_exists() {
	if (typeof arguments[0] == 'object') {  
		var criterion = arguments[0].constructor.toString().match(/array/i); 
 		return (criterion != null);  
	}
	return false;
}// Array_Exists() End


//Default lib functions
function lib_create_obj_0001(_obj_typ,_id,_id_parent,_class,_zIndex,_position,_backgroundColor,_backgroundImage,_border,_visibility,_left,_top,_width,_height,_onmousedown,_onclick,_innerHTML,_src,_scrolling,_frameBorder,_cursor,_fontFamily,_fontWeight,_fontSize,_color,_display,_opacity_value){
	var obj;
	if(document.createElement&&(obj=document.createElement(_obj_typ))){
		obj.className=_class;
		obj.style.zIndex=_zIndex;
		obj.name=obj.id=_id;
		obj.style.position=_position;
		obj.style.left=_left;
		obj.style.top=_top;
		obj.style.width=_width;
		obj.style.height=_height;
		obj.style.backgroundColor=_backgroundColor;
		obj.style.visibility=_visibility;
		obj.style.cursor=_cursor;
		if("div"==_obj_typ.toLowerCase()){
			obj.style.fontFamily =_fontFamily;
			obj.style.fontWeight=_fontWeight;
			obj.style.fontSize=_fontSize;
			obj.style.color=_color;
			obj.style.display=_display;
			obj.style.border=_border;
			obj.innerHTML=_innerHTML;
			obj.style.backgroundImage="url('"+_backgroundImage+"')";
			obj.onmousedown=function(event){eval(_onmousedown);}
			obj.onclick=function(event){eval(_onclick);}
			if(_opacity_value<10){
				obj.style.opacity=_opacity_value/10; // CSS3 compliant (Moz 1.7alpha+, Safari 1.2+)
				obj.style.MozOpacity=_opacity_value/10; // older Mozilla, Netscape 7.x
				obj.style.filter='alpha(opacity='+_opacity_value*10+')'; // Internet Exploder
			}
		}
		if("iframe"==_obj_typ.toLowerCase()){
			obj.src=_src;
			obj.scrolling=_scrolling;

			obj.frameBorder=_frameBorder;
		}
		if("body"==_id_parent.toLowerCase()){document.body.appendChild(obj);}
		else{document.getElementById(_id_parent).appendChild(obj);}
	}
	return obj;
}


// PopUp Layer Close
function PopUp_Layer_Close(_id){
	lib_close((_id+"I_F"));lib_close((_id+"I_W"));
	lib_close((_id+"W_R"));lib_close((_id+"N_R"));lib_close((_id+"E_R"));lib_close((_id+"S_R"));
	lib_close((_id+"NW_R"));lib_close((_id+"SW_R"));lib_close((_id+"NE_R"));lib_close((_id+"SE_R"));
	lib_close((_id+"TT_L"));lib_close((_id+"CB_L"));lib_close((_id+"MAXB_L"));lib_close((_id+"MIN_L"));
	lib_close((_id+"T_B"));lib_close((_id+"M_L"));lib_close((_id+"CL_L"));
	hideFadeScreen();
}// PopUp Layer Close End


// lib close
function lib_close(_id){
	if(null!=document.getElementById(_id)){document.getElementById(_id).parentNode.removeChild(document.getElementById(_id));}
}// lib close End


// Mouse Movement Begin
// Determine browser and version.
function Browser() {
  var ua, s, s2, i;
  this.isIE    = false;
  this.isNS    = false;
  this.version = null;
  ua = navigator.userAgent;
  s = "MSIE";
  s2 = "Opera";
  if (((i = ua.indexOf(s)) >= 0) || ((i = ua.indexOf(s2)) >= 0)) {
    this.isIE = true;
    this.version = parseFloat(ua.substr(i + s.length));
    return;
  }
  s = "Netscape6/";
  if ((i = ua.indexOf(s)) >= 0) {
    this.isNS = true;
    this.version = parseFloat(ua.substr(i + s.length));
    return;
  }

  // Treat any other "Gecko" browser as NS 6.1.

  s = "Gecko";
  if ((i = ua.indexOf(s)) >= 0) {
    this.isNS = true;
    this.version = 6.1;
    return;
  }
}

var browser = new Browser();
var nn = 500;
// Global object to hold drag information.
var dragObj = new Object();
dragObj.zIndex = 500;

function dragStart(event, id) {
  nn++;
  document.getElementById(id).style.zIndex = nn;
  var el;
  var x, y;
  // If an element id was given, find it. Otherwise use the element being
  // clicked on.
  if (id)
    dragObj.elNode = document.getElementById(id);
  else {
    if (browser.isIE)
      dragObj.elNode = window.event.srcElement;
    if (browser.isNS)
      dragObj.elNode = event.target;
    // If this is a text node, use its parent element.
    if (dragObj.elNode.nodeType == 3)
      dragObj.elNode = dragObj.elNode.parentNode;
  }

  // Get cursor position with respect to the page.
  if (browser.isIE) {
    x = window.event.clientX + document.documentElement.scrollLeft
      + document.body.scrollLeft;
    y = window.event.clientY + document.documentElement.scrollTop
      + document.body.scrollTop;
  }
  if (browser.isNS) {
    x = event.clientX + window.scrollX;
    y = event.clientY + window.scrollY;
  }

  // Save starting positions of cursor and element.

  dragObj.cursorStartX = x;
  dragObj.cursorStartY = y;
  dragObj.elStartLeft  = parseInt(dragObj.elNode.style.left, 10);
  dragObj.elStartTop   = parseInt(dragObj.elNode.style.top,  10);

  if (isNaN(dragObj.elStartLeft)) dragObj.elStartLeft = 0;
  if (isNaN(dragObj.elStartTop))  dragObj.elStartTop  = 0;

  // Update element's z-index.

  dragObj.elNode.style.zIndex = ++dragObj.zIndex;

  // Capture mousemove and mouseup events on the page.

  if (browser.isIE) {
    document.attachEvent("onmousemove", dragGo);
    document.attachEvent("onmouseup",   dragStop);
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS) {
    document.addEventListener("mousemove", dragGo,   true);
    document.addEventListener("mouseup",   dragStop, true);
    event.preventDefault();
  }
}

function dragGo(event) {

  var x, y;

  // Get cursor position with respect to the page.

  if (browser.isIE) {
    x = window.event.clientX + document.documentElement.scrollLeft
      + document.body.scrollLeft;
    y = window.event.clientY + document.documentElement.scrollTop
      + document.body.scrollTop;
  }
  if (browser.isNS) {
    x = event.clientX + window.scrollX;
    y = event.clientY + window.scrollY;
  }

  // Move drag element by the same amount the cursor has moved.

  dragObj.elNode.style.left = (dragObj.elStartLeft + x - dragObj.cursorStartX) + "px";
  dragObj.elNode.style.top  = (dragObj.elStartTop  + y - dragObj.cursorStartY) + "px";

  if (browser.isIE) {
    window.event.cancelBubble = true;
    window.event.returnValue = false;
  }
  if (browser.isNS)
    event.preventDefault();
}

function dragStop(event) {

  // Clear the drag element global.

  dragObj.elNode = null;

  // Stop capturing mousemove and mouseup events.

  if (browser.isIE) {
    document.detachEvent("onmousemove", dragGo);
    document.detachEvent("onmouseup",   dragStop);
  }
  if (browser.isNS) {
    document.removeEventListener("mousemove", dragGo,   true);
    document.removeEventListener("mouseup",   dragStop, true);
  }
}// Mouse Movement End


function showFadeScreen()
{

	var objBody = document.getElementsByTagName("body").item(0);
	var objOverlay = document.createElement("div");

	objOverlay.setAttribute('id','fade_screen');
//	objOverlay.onclick = function () {hideFadeScreen(); return false;}
	objOverlay.style.display = 'none';
	objOverlay.style.position = 'fixed';
	objOverlay.style.top = '0';
	objOverlay.style.left = '0';
	objOverlay.style.zIndex = '91';
 	objOverlay.style.width = '100%';//;'100%';
	objBody.insertBefore(objOverlay, objBody.firstChild);

//	objOverlay.style.height = (docheight + 'px');
	objOverlay.style.height = '100%';
	objOverlay.style.display = 'block';




}
                      
function hideFadeScreen() {

 	objOverlay = document.getElementById('fade_screen');
	objOverlay.style.display = 'none';
	document.onkeypress = '';


}


function loadXML(url,reply) {
	req = false;
    // branch for native XMLHttpRequest object
    if(window.XMLHttpRequest && !(window.ActiveXObject)) {
    	try {
			req = new XMLHttpRequest();
        } catch(e) {
			req = false;
        }
    // branch for IE/Windows ActiveX version
    } else if(window.ActiveXObject) {
       	try {
        	req = new ActiveXObject("Msxml2.XMLHTTP");
      	} catch(e) {
        	try {
          		req = new ActiveXObject("Microsoft.XMLHTTP");
        	} catch(e) {
          		req = false;
        	}
		}
    }
	if(req) {
		req.onreadystatechange = reply;
		req.open("GET", url, true);
		req.send("");
	}
}

