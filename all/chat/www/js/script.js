
function checkTime(i)
{
    if (i<10)  {i="0" + i}
    return i
}
    
window.encodeURIComponent = function(str){
    var trans = [];
    for (var i = 0x410; i <= 0x44F; i++) trans[i] = i - 0x350; // ?-? -п
    trans[0x401] = 0xA8; // р
    trans[0x451] = 0xB8; // с
    var ret = [];
    for (var i = 0; i < str.length; i++){
          var n = str.charCodeAt(i);
          if (typeof trans[n] != 'undefined') n = trans[n];
          if (n <= 0xFF) ret.push(n);
    }
    return escape(String.fromCharCode.apply(null, ret)).replace(/\+/g, '%2B'); // +
}

window.decodeURIComponent = function(str){
    return unescape(str);
}

var trans=[];
    for(var i=0x410;i<=0x44F;i++)
    trans[i]=i-0x350;
trans[0x401]=0xA8;
trans[0x451]=0xB8;
var escapeOrig=window.escape;
    window.escape=function(str){
    var ret=[];
        for(var i=0;i<str.length;i++){
        var n=str.charCodeAt(i);
            if(typeof trans[n]!='undefined')
            n = trans[n];
            if (n <= 0xFF)
            ret.push(n);
        }
    return escapeOrig(String.fromCharCode.apply(null,ret));
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



function parseAction(type,XML) {
    if(type == 'join') {
	if(inchat>0) {
	    if(chat_list_status == 0) {
		setTimeout("chat_list_cron()", 100);
		chat_list_status=1;
	    } else {
		chat_list();
	    }
	}

	
	if(reload_status == 0) {
	    setTimeout("reload_cron()", 100);
	    reload_status=1;
	} else {
	    reload();
	}

	if($("place_id",XML).text())
		current_place=$("place_id",XML).text();
	else if($("check/place",XML).text())
		current_place=$("check/place",XML).text();
	else 
		current_place=0;
		
	$("#chat_text").html('<div align="center">---=]<b>W</b><b style="color:red">elcome T</b><b>o</b>, не шумите!!![=---</div>');
	$("#chat").show("slow");
	$("#chat_select").hide("slow");
	$('input',"#add_msg_form").focus();
   } else  if(type =='status') {
	switch(XML) {
	case 'denied':
		$("#llogin").css("border","1px solid red");
		$("#lpass").css("border","1px solid red");
		break;
	default:
		break;
	 }

   } else  if(type =='char_select') {

	$("#char").show("slow");

//	$("#login").hide("slow");
	
	if($("count",XML).text() == 0) {
		$("#char_select_div").html('<b style="color:red">вы должны создать нового персонажа</b>');
	} else {

	    $("#char_select_div").html("");
	    $("chars/char",XML).each(function(i){
		$("<li style=\"height:20px;width:150px;cursor:pointer;\" id=\""+"char_"+$("id",this).text()+"\" onclick=\"return char_select('"+$("id",this).text()+"')\" >"+$("name",this).text()+"</li>").appendTo("#char_select_div");
		$("#char_"+$("id",this).text()).contextMenu("#myMenu", {
		    bindings : {
	    	        "#delete" : function(t) {
	    			var y=window.confirm("вы уверены что хотите удалить чар - "+$(t).text()+" ?");
						if(y)  { 
							       $.ajax({
								    url:'/call/registration',
								    type: 'POST',
								    dataType:'xml',
								    data:{'call':'char_del','char':$(t).text()},
									success: function(XML) {
												
												if($("status",XML).text() == 'failed') {
													alert('char login del failed');
												} else if($("status",XML).text() == 'ok') {
													char_list();
												}
									}
							});

						}
	        	}
		    }
		});
		
	    });
	
	}
    


   }



}

function Expire(num) {
	if($("#chat_text/div").length > num) while($("#chat_text/div").length>num) $($("#chat_text/div")[0]).remove();
}

function AddMsg(name,time,message,private) {
	var d = new Date(time * 1000);
	if(private == 1) {
		$('<div>&raquo;&nbsp;<b><i>'+name+'</i></b>&nbsp;('+checkTime(d.getHours())+':'+checkTime(d.getMinutes())+':'+checkTime(d.getSeconds())+') - '+message+'</div>').appendTo("#chat_text")
	} else if(private == 2) {
		$('<div>*&nbsp;<b><i>'+name+'</i></b>&nbsp;('+checkTime(d.getHours())+':'+checkTime(d.getMinutes())+':'+checkTime(d.getSeconds())+') - '+message+'</div>').appendTo("#chat_text")
	} else {
		$('<div><b>'+name+'</b>&nbsp;('+checkTime(d.getHours())+':'+checkTime(d.getMinutes())+':'+checkTime(d.getSeconds())+') - '+message+'</div>').appendTo("#chat_text")
	}
	Expire(100);
	document.getElementById('chat_text').scrollTop=document.getElementById('chat_text').scrollHeight;
	$('input',"#add_msg_form").focus();
}


var ChatPlace=new Array();

function parseChat(type,XML) {

   if(type =='place') {
//	alert(current_place);

   $(inchat == 0 ? "#chat_select_div" : "#chat_select_list").html('');
   $("chat/place",XML).each(function(i){
	$("<span onclick=\"return chat_join('"+$("id",this).text()+"')\" style=\"cursor:pointer\">"+(current_place == $("id",this).text() ? "<b>" : "")+$("name",this).text()+(current_place == $("id",this).text() ? "</b>" : "")+"("+$("users",this).text()+")</span><br/>").appendTo(inchat == 0 ? "#chat_select_div" : "#chat_select_list")
   });

   
   } else if(type == 'user') {

	   $('#chat_user').html('');
	   $("chat/user",XML).each(function(i){
		$("<div id=\""+$("id",this).text()+"\" name=\""+$("name",this).text()+"\"><b>"+$("name",this).text()+"</b> ("+$("ping",this).text()+")"+"</div>'").appendTo("#chat_user");
		$("#"+$("id",this).text()).contextMenu("#myMenu", {
		    bindings : {
	    	        "#private" : function(t) {
	    			var y=window.prompt("что хотите написать?");
				if(y)  { add('/MSG'+' <'+$(t).attr("name")+'> '+y); }
	        	},
	    	        "#profile" : function(t) {
	    			var y=window.prompt("что хотите написать?");
				if(y)  { add('/MSG'+' <'+t.name+'> '+y); }
	        	}
		    }
		});
	   });


   } else if(type == 'msg') {

	   $("chat/msg",XML).each(function(i){AddMsg($("name",this).text(),$("time",this).text(),$("string",this).text(),$("private",this).text());});

   }

}
	 

function doXML(XML) {
//    alert($(XML).html());
	$("login/status",XML).each(function(i){ parseAction('status',$(this).text()); });

	$("reload",XML).each(function(i){ document.location=$(this).text(); });
//	if($("reload",XML).text()) { alert($("reload",XML).text()); };

	$("check/place",XML).each(function(i){ if($("check/place",XML).text() > 0) parseAction('join',XML); });

	if($("action",XML).text() == 'join') parseAction('join',XML);

	if($("action",XML).text() == 'char_select') parseAction('char_select',XML);

	if($("chat/place",XML).length > 0) parseChat('place',XML);

	if($("user",XML).length > 0) parseChat('user',XML);

	if($("chat/msg",XML).length > 0) parseChat('msg',XML);

}

function login(login,pass){
	if(pass.length >0 && login.length >0) {
		$.ajax({url:'/call/chat', type: 'POST',dataType:'xml',data:{ 'call':'login','login': login,'password':pass},success: doXML });
	} else {
		if(login.length == 0)
		     $('#llogin').css("border","1px solid red"); 
		 if(pass.length == 0)
		     $('#lpass').css("border","1px solid red"); 
	}
	return false;
}

function char_select(char){
	$.ajax({url:'/call/chat', type: 'POST',dataType:'xml',data:{ 'call':'char_select','login':char},success: doXML });
	return false;
}

function char_list(){
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'char_list'},success: doXML });
        return false;
}



function login_guest(login,pass){
	$.ajax({url:'/call/chat', type: 'POST',dataType:'xml',data:{ 'call':'login_guest'},success: doXML });
	return false;
}


function refreshcap() {
		$('#captcha').attr('src','');
		$('#captcha').attr('src','/captcha.png?'+Math.random());
}


function chat_join(id){
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'join','place':id},success: doXML });
        return false;
}



function get_status(){
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'status'},success: doXML });
        return false;
}


function reload() {
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'fresh'},success: doXML });
}
function reload_cron() {
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'fresh'},success: doXML });
    	setTimeout('reload_cron()', 7000);
}

function chat_list(){
        $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'getplace'},success: doXML });
        return false;
}

function chat_list_cron() {
	chat_list();
	if(inchat == 1)  setTimeout('chat_list_cron()', 70000);
}

function add(msg) {
	if(msg.length > 0 && msg.length < 150) {
            $.ajax({url:'/call/chat', type: 'POST', dataType: "xml",data:{ 'call':'addmsg','msg':msg},success: function (i) { $('input',"#add_msg_form").val('');doXML(i); }});
        } else if(msg.length > 150) {
	    alert('Нельзя писать сообщения длинной больше 150 символов');
	}
        return false;
}



var inchat=0;
var current_place=0;

var reload_status=0;

var chat_list_status=0;
