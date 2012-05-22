function popup_layer(url,title,w,h)
{

	var pWidth=w;
	var pHeigth=h;
	var pX=((docwidth/2-15)-pWidth/2);
	PopUp_Layer("Layer_Reg", "iframe", title, url, pX, 50, pWidth, pHeigth,"Registration");

}


function register_form(title)
{

	var pWidth=800;
	var pHeigth='600';
	var pX=((docwidth/2-15)-pWidth/2);
	PopUp_Layer("Layer_Reg", "iframe", title, BASEURI+"signup/?popup", pX, 50, pWidth, pHeigth,"Registration");

}

function getPageSizeWithScroll(){
	if (window.innerHeight && window.scrollMaxY) {// Firefox
		yWithScroll = window.innerHeight + window.scrollMaxY;
		xWithScroll = window.innerWidth + window.scrollMaxX;
	} else if (document.body.scrollHeight > document.body.offsetHeight){ // all but Explorer Mac
		yWithScroll = document.body.scrollHeight;
		xWithScroll = document.body.scrollWidth;
	} else { // works in Explorer 6 Strict, Mozilla (not FF) and Safari
		yWithScroll = document.body.offsetHeight;
		xWithScroll = document.body.offsetWidth;
  	}
	arrayPageSizeWithScroll = new Array(xWithScroll,yWithScroll);
	//alert( 'The height is ' + yWithScroll + ' and the width is ' + xWithScroll );
	return arrayPageSizeWithScroll;
}



var Window = '';
function WindowOpen(PicSrcPath, width, height){
	if (!Window.closed && Window.location)
	{
		Window.location.href = url;
	}
	else
	{
	Window = window.open(PicSrcPath,'_pop','channelmode=0,toolbar=0,location=0,directories=0,status=0,menubar=0,scrollbars=1,resizable=0');
	Window.resizeTo(width, height);
	Window.moveTo( (screen.width - width) / 2, 100);
	Window.focus();
	if (!Window.opener) Window.opener = document;
	}
}


function alerter(lang,name)
{
	if(lang == 'ru')
	{
		var enter='Пожалуйста введите';
	} else {
		var enter='Please enter';
	}

	var form = document.forms[name].elements;
	re=/Error$/;
	var el;

	for(i=0;form[i];i++)
	{

		if((form[i].nodeName == 'INPUT' || form[i].nodeName == 'SELECT') && form[i].className.match(re))
		{

		    if(form[i].type != 'radio' || 1==1)
		    {
			if(el=document.getElementById(form[i].id +'_error'))
			{
				el.innerHTML='<span title=\''+enter+', '+form[i].title+'\' style=\'font-weight:bold;font-size:1.5em;color:red\'><blink>?</blink></span>';
			}
		    }
		}			
	}

}
function changeOptions(idName,optionsID)
{
 document.getElementById(idName).options[optionsID].selected=true;
}




function reload_print(url,sec)
{



    var html_doc = document.getElementsByTagName('body').item(0);
    var div= document.createElement('div');
    div.id='reload';
    div.style.position='fixed';
    div.style.align='center';
    div.style.width='200px';
    div.style.top='45%';
    div.style.border='1px dashed black';
    div.style.left='45%';
    div.style.zIndex='666';
    div.style.height='100px';
    div.style.verticalAlign='middle';
    div.style.textAlign='center';
    html_doc.appendChild(div);
    window.status='Done';
    reload(url,sec);

}

function reload(url,sec) {
if(sec >= 0) { document.getElementById('reload').innerHTML='<p style="padding-top:35px;font-family:verdana"><h1>'+sec+'</h1></p>';sec--;setTimeout('reload("'+url+'","'+sec+'")', 1000); }
else { document.getElementById('reload').innerHTML='<p style="padding-top:35px;font-family:verdana">reloaded to: '+url+'<br/> <h1>please wait</h1></p>';setTimeout('document.location.replace("'+url+'")', 1000); }
}


function r2eload_print(url,sec)
{



    var html_doc = document.getElementsByTagName('head').item(0);
    var div= document.createElement('div');
    div.style.position='fixed';
    div.style.align='center';
    div.style.valign='center';

	document.write('<div align="center" id="timeout"></div>');
	document.write('<scr'+'ipt>');
	document.write('var sec='+sec+';');
	document.write('function reload() {');
	document.write('if(sec >= 0) { document.getElementById(\'timeout\').innerHTML=sec;sec--;setTimeout(\'reload()\', 1000); }');
	document.write('else { document.getElementById(\'timeout\').innerHTML=\'reloaded to: '+url+', please wait\';setTimeout(\'document.location.replace("'+url+'")\', 1000); }');
	document.write('}');
	document.write('window.status=\'Done\';');
	document.write('reload();');
	document.write('<'+'/sc'+'ript>');

}

function dump(arr,level) {
	var dumped_text = "";
	if(!level) level = 0;

	//The padding given at the beginning of the line.
	var level_padding = "";
	for(var j=0;j<level+1;j++) level_padding += "    ";

	if(typeof(arr) == 'object') { //Array/Hashes/Objects
		 for(var item in arr) {
			  var value = arr[item];
 
			  if(typeof(value) == 'object') { //If it is an array,
				   dumped_text += level_padding + "'" + item + "' ...\n";
				   dumped_text += dump(value,level+1);
			  } else {
				   dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
	  		  }
		}
	} else { //Stings/Chars/Numbers etc.
		 dumped_text = "===>"+arr+"<===("+typeof(arr)+")";
	}
	return dumped_text;
} 

function Int2Print(Int,level,drob) {

 if(Lang == 'ru')  { drobdelim='.'; intdelim=','; }
 else { drobdelim='.'; intdelim=','; }
 Int = Math.round(Int * Math.pow(10, drob)) / Math.pow(10, drob);
 Int = Int + '';
 f = Int.split('.');
 if (!f[0]) {
  f[0] = '0';
 }
 if (!f[1]) {
  f[1] = '';
 }
 if (f[1].length < drob) {
  g = f[1];
  for (i=f[1].length + 1; i <= drob; i++) {
   g += '0';
  }
  f[1] = g;
 }
 if(intdelim != '' && f[0].length > level) {
  h = f[0];
  f[0] = '';
  for(j = level; j < h.length; j+=level) {
   i = h.slice(h.length - j, h.length - j + level);
   f[0] = intdelim + i +  f[0] + '';
  }
  j = h.substr(0, (h.length % level == 0) ? level : (h.length % level));
  f[0] = j + f[0];
 }
 drobdelim = (drob <= 0) ? '' : drobdelim;
 return f[0] + drobdelim + f[1];

/*	int=int+'';
	var new_int='';
	var ii=0;
	for(var i=int.length;i>0;i--) {
		ii++
		if(ii>level) {
			new_int=','+new_int;
			ii=1;
		}
		new_int=int.substr(i-1,1)+new_int;
	}

	return new_int;
*/
}

function Print2Int(print) {
	print=print+'';
	print=parseInt(print.replace(/,/g,''));
	print=print>0 ? print : 0;
  
   return print;
}


/**
*  removes children of a node recursively
*  @param node
*     The DOM node whose children will be removed
**/
function removeChildren(node) {
   var childCount = node.childNodes.length;
   for (var i=0; i<childCount; i++) {
      var child = node.childNodes[0];
      node.removeChild(child);
   }
}


/**
*  @return true if the element with the given name is currently visible
*  @param elementID - the 'id of the element in the web page
*/
function isElementVisible(elementID) {
   var element = document.getElementById(elementID);
   if (element.style.visibility != 'hidden' &&
      element.style.display != 'none') {
      return true;
   } else {
      return false;
   }
}

/**
*  toggles the visibility of an element with the given name if showElement is true
*  @param elementID - the 'id of the element in the web page
*  @param show - show the element if this value is true, otherwise hide the element
*
*/
function toggleElementVisibility(elementID, show) {
   if (show == null)
      show = !isElementVisible(elementID);
   if (show == true)
      showElement(elementID);
   else
      hideElement(elementID);
}

/**
*  hides an element with the given ID
*  @param elementID - the 'id' of the element in the web page
*/
function hideElement(elementID) {
   var element = document.getElementById(elementID);
   if (element != null) {
      element.style.display = 'none';
      element.style.visibility = 'hidden';
   }
}

/**
* shows an element with the given ID
* @param elementID - the 'id' of the element in the web page
* @param style - the 'style' of the element, uses 'block' if not specified
*/
function showElement(elementID, style) {
   if (style == null)
      style = 'block';
   var element = document.getElementById(elementID);
   if (element != null) {
      element.style.display = style;
      element.style.visibility = 'visible';
   }
}

/**
* disable/enable button with the given ID
* @param elementID - the 'id' of the element in the web page
* @param false/true 
*/
function buttonSwitch(elementID, Set) {
   var element = document.getElementById(elementID);

   if (element != null) {
	if(Set == 'on')
	      element.disabled = false;
	else
	      element.disabled = true;
   }
}

/**
*  trims whitespace from the string
*  @param s - the string to trim
*  @return - the trimmed string
*/
function trim(s) {
  while (s.substring(0,1) == ' ') {
    s = s.substring(1,s.length);
  }
  while (s.substring(s.length-1,s.length) == ' ') {
    s = s.substring(0,s.length-1);
  }
  return s;
}


/*****************************************************/
/************* Date Functions ************************/
/*****************************************************/

/**
* @param daysAgo
*    number of days to subtract from date
* @return
*   new javascript date object representing the current date, minus daysAgo
*/
function dateAdd(daysAhead) {
   var date = new Date();
   var days = date.getDate();
   date.setDate(days + daysAhead);
   return date;
}

/**
* adds a text node to an existing element, given the elementID
* @param elementID
*     the elementID of the existing DOM node
* @param text
*     the text to add to the node
**/
function setText(elementID, text) {

  var element = document.getElementById(elementID);
  var oldText = element.childNodes[0];
  var newText = document.createTextNode(text);

  if (oldText == null)
    element.appendElement(newText);
  else
    element.replaceChild(newText, oldText);
}

/**************************************************/
/********** Form Functions ************************/
/**************************************************/
function clearSearchBox(searchBox, defaultValue)
{
   if (searchBox.value == defaultValue)
   {
      searchBox.value = '';
   }
}

function getFormElementValue(element) {

  var value;

  if (element == null)
   alert(elementID + " is null");

  /* select box */
  if (element.type == "select-one") {
     var options = element.options;
     value = options[options.selectedIndex];
  }

  /* text or hidden */
  if (element.type == "hidden" ||
       element.type == "text" ||
       element.type == "textarea")
     value = element.value;

}

/**
*  initializes a form element based on the id and value.
*/
function setFormElementValue(elementID, value) {
  var element = document.getElementById(elementID);

  if (element == null)
  alert(elementID + " is null");

  /* select box */
  if (element.type == "select-one") {
     var options = element.options;
     for (var i=0; i<options.length; i++) {
       if (options[i].value == value)
         options[i].selected = true;
     }
  }

  /* text or hidden */
  if (element.type == "hidden" ||
       element.type == "text" ||
       element.type == "textarea")
     element.value = value;

  /* checkbox */
  if (element.type == "checkbox") {
    if (value==true)
       element.checked = true;
    else
       element.checked = false;
  }

  /* radio box */
  if (element.type == "radio") {
    if (value = element.value)
       element.checked = true;
    else
       element.checked = false;
  }

}

/**
 * changes the text of a button jsp include
 */
function changeButtonText(newText) {
   var newNode = document.createTextNode(newText);
   var button = document.getElementById("saveSearchFormSubmit")
   var buttonDiv = button.childNodes[0];
   var oldNode;
   for (var i=0; i<buttonDiv.childNodes.length; i++) {
      if (buttonDiv.childNodes[i].data != null)
        oldNode = buttonDiv.childNodes[i];
   }
   buttonDiv.replaceChild(newNode, oldNode);
}

/***************************************************/
/************ Cookie Functions *********************/
/***************************************************/

/* creates a cookie with the given name and value, set to expire after days */
function createCookie(name,value,days)
{
   if (days)
   {
      var date = new Date();
      date.setTime(date.getTime()+(days*24*60*60*1000));
      var expires = "; expires="+date.toGMTString();
   }
   else var expires = "";
   document.cookie = name+"="+value+expires+"; path=/";
}

/* reads the cookie with the given name */
function readCookie(name)
{
   var nameEQ = name + "=";
   var ca = document.cookie.split(';');
   for(var i=0;i < ca.length;i++)
   {
      var c = ca[i];
      while (c.charAt(0)==' ') c = c.substring(1,c.length);
      if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
   }
   return null;
}

/* deletes the cookie with the given name */
function eraseCookie(name)
{
   createCookie(name,"",-1);
}


function isValueInArray(array, value)
{
   for (var index = 0; index < array.length; index++)
      if (array[index] === value)
         return true;

   return false;
}

function number_format(a, b, c, d) {
 a = Math.round(a * Math.pow(10, b)) / Math.pow(10, b);
 e = a + '';
 f = e.split('.');
 if (!f[0]) {
  f[0] = '0';
 }
 if (!f[1]) {
  f[1] = '';
 }
 if (f[1].length < b) {
  g = f[1];
  for (i=f[1].length + 1; i <= b; i++) {
   g += '0';
  }
  f[1] = g;
 }
 if(d != '' && f[0].length > 3) {
  h = f[0];
  f[0] = '';
  for(j = 3; j < h.length; j+=3) {
   i = h.slice(h.length - j, h.length - j + 3);
   f[0] = d + i +  f[0] + '';
  }
  j = h.substr(0, (h.length % 3 == 0) ? 3 : (h.length % 3));
  f[0] = j + f[0];
 }
 c = (b <= 0) ? '' : c;
 return f[0] + c + f[1];
}

function valueClear (ptr) {
	if(ptr.value==ptr.title) {ptr.value=''}

}
function valueFill (ptr) {
	if(ptr.value=='') {ptr.value=ptr.title}

}

