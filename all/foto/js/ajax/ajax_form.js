var original=new Array();
var original_title=new Array();
var original_border=new Array();
var query=new Array();

var select_id=document.getElementsByTagName('select');
var input_id=document.getElementsByTagName('input');


function changeBorder(evt){
	var e_out;
	var ie_var = "srcElement";
	var moz_var = "target";

	evt[moz_var] ? e_out = evt[moz_var] : e_out = evt[ie_var];

	if(evt.type == "mouseover")
			e_out.style.border="1px solid";

	else if(evt.type =="mouseout")
			e_out.style.border=original_border[e_out.name];

}


function checkInput(evt)
{

	var e_out;
	var ie_var = "srcElement";
	var moz_var = "target";
	evt[moz_var] ? e_out = evt[moz_var] : e_out = evt[ie_var];


	if(evt.type == "keyup" || evt.type == "change")
	{
		 if(e_out.type == 'text' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name] !=  e_out.value)
		 {
			query[e_out.name]=e_out.value;
			e_out.style.border="1px solid #D4D1C6";
			original_border[e_out.name]=e_out.style.border;
		 }
		 else if(e_out.type == 'text' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name] ==  e_out.value)
		 {
			delete(query[e_out.name]);
			original_border[e_out.name]="1px solid #D4D1C6";
		 }
		 else if(e_out.type == 'select-one' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] !=  e_out.value)
		 {
			query[e_out.name]=e_out.value;
		 }
		 else if(e_out.type == 'select-one' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] ==  e_out.value)
		 {
			delete(query[e_out.name]);
		 }
		 else if(e_out.type == 'select-multiple' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ))
		 {
			delete(query[e_out.name])
			query[e_out.name]=new Array();
			for(var opt_i=0;e_out.options[opt_i];) {
				if(e_out.options[opt_i].selected == true) {
					query[e_out.name][e_out.options[opt_i].value]=e_out.options[opt_i].value;
				}
				opt_i++;
			}
		 }
		 else if(e_out.type == 'radio' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name] !=  e_out.value)		 
		 {
			query[e_out.name]=e_out.value;
		 }
		 else if(e_out.type == 'radio' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name] ==  e_out.value)
		 {
			delete(query[e_out.name]);
		 }
		 else if(e_out.type == 'checkbox' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name]!=( e_out.checked ? 'on' : 'off'))
		 {
			query[e_out.name]=( e_out.checked ? 'on' : 'off');
		 }
		 else if(e_out.type == 'checkbox' && (  e_out.id == 'form' || e_out.name.indexOf("form[") > -1 ) && original[e_out.name] && original[e_out.name]==( e_out.checked ? 'on' : 'off'))
		 {
			delete(query[e_out.name]);
		 }
	 count=0;
	 for(var item in query) {
	  count++;
	 } 

	if(document.getElementById('submit')) {	if(count == 0) 	document.getElementById('submit').disabled=true; else document.getElementById('submit').disabled=false;	}

	}
}


function initform()
{
	if(document.getElementById('submit')) document.getElementById('submit').disabled=true;


	for(var i=0;input_id[i];i++)
	{
	if(input_id[i].id == 'form' || input_id[i].name.indexOf("form[") > -1 ) {
	  if(input_id[i].type != 'radio' ) {

		if(window.addEventListener){ // Mozilla, Netscape, Firefox
			input_id[i].addEventListener('mouseover', changeBorder, false);
			input_id[i].addEventListener('keyup', checkInput, false);
			input_id[i].addEventListener('mouseout', changeBorder, false);
		} else { // IE
			input_id[i].attachEvent('onkeyup', checkInput);
			input_id[i].attachEvent('onmouseover', changeBorder);
			input_id[i].attachEvent('onmouseout', changeBorder);
		}
	  }  else {	
		if(window.addEventListener){ // Mozilla, Netscape, Firefox
			input_id[i].addEventListener('change', checkInput, false);
		} else { // IE
			input_id[i].attachEvent('onchange', checkInput);

		}
	 }	
		if(!query[input_id[i].name]) {
			input_id[i].style.border="1px solid #D4D1C6";
		}
		if(input_id[i].type == 'checkbox') {
			original[input_id[i].name]=( input_id[i].checked ? 'on' : 'off');
		} else if(input_id[i].type == 'radio' ) {
			if(input_id[i].checked )
				original[input_id[i].name]=input_id[i].value;
		} else {
			original[input_id[i].name]=input_id[i].value?input_id[i].value : ' ';
		}
		original_title[input_id[i].name]=input_id[i].title;
		original_border[input_id[i].name]=input_id[i].style.border
	  }
	}

	for(var i=0;select_id[i];i++)
	{
		if(window.addEventListener){ // Mozilla, Netscape, Firefox
			select_id[i].addEventListener('change', checkInput, false);
		} else { // IE
			select_id[i].attachEvent('onchange', checkInput);

		}

		if(!query[select_id[i].name]) {
			select_id[i].style.border="1px solid #D4D1C6";
		}
		original[select_id[i].name]=select_id[i].selectedIndex;
		original_title[select_id[i].name]=select_id[i].title;
		original_border[select_id[i].name]=select_id[i].style.border
	}

	query=new Array();

}
function resetform()
{
	if(document.getElementById('submit'))
	document.getElementById('submit').disabled=true;
	for(var i=0;input_id[i];i++)
	{
		if(original[input_id[i].name])
		{
			if(input_id[i].type == 'text')
				input_id[i].value=original[input_id[i].name];
			input_id[i].style.border="1px solid #D4D1C6";
		}
		if(req.responseJS[input_id[i].name] && query[input_id[i].name])
		{
				input_id[i].style.border="2px solid";
				input_id[i].style.borderColor="red";
		}
		else if(!req.responseJS[input_id[i].name] && query[input_id[i].name])
		{
			input_id[i].style.border="2px solid";
			input_id[i].style.borderColor="green";
		}
		original_border[input_id[i].name]="1px solid #D4D1C6";
	}
}



var reply_request=function(){
	 var err="";
	 if(req.responseJS.result == 'ERR') {
		 for(var item in query) {
			if(typeof(req.responseJS[item]) != 'undefined') {
				err=err+string['ERROR_FORM_ENTRY']+': '+original_title[item]+', '+string['ERROR_YOU_ENTER']+' - '+req.responseJS[item]+'\n';
			}  else	{
				original[item]=query[item];

			}
		 } 

			alert(err);
			resetform();
	 } else	 if(req.responseJS.result == 'TMP') {
			alert(string['TRY_SEC']);
	 } else	 if(req.responseJS.result == 'ALREADY') {
			alert(string['TRY_ALREADY']);
	 } else {

		 for(var item in query) {
			original[item]=query[item];
		 } 

		resetform();

		initform();
	 }
}

function request()
{
	 ajax_query('?ajax',query,reply_request);

} 
initform();
