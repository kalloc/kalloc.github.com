<?
header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Last-Modified: '.gmdate().'D, d M Y H:i:s'.' GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Pragma: no-cache');
header('Content-Type: text/xml');

include('var.php');
include('inc.php');


define('DELIM',"  ");
function do_xml($ptr,$del=0) {
	$ret='';
	if(is_array($ptr))  
          	 foreach($ptr as $key=>$val) 
				$ret.=str_repeat(DELIM,$del).'<'.$key.'>'."\n".(is_array($val) ? do_xml($val,$del+1) : str_repeat(DELIM,$del+1).'<![CDATA['.$val."]]>\n").str_repeat(DELIM,$del).'</'.$key.'>'."\n";
	 else 	 
		$ret=$ptr;

	return $ret;

}


/*
client_exec('USER_JOIN',pack("i*",1).pack("i*",1));
client_exec('USER_LOGIN',"n2ckname\x00pass");
client_exec('USER_JOIN',pack("i*",2).pack("i*",1));
client_exec("USER_LIST_PLACE",pack("i*",1));
client_exec("USER_LIST",pack("i*",1));
client_exec('USER_LOGOUT',pack("i*",1)."n1ckname");


*/
//setcookie('PHPSESSID','gm0k7pua2qqu8ekgcrnlscs5l1',time()+3600,'/');
//session_start();
	

function getInt(&$buff) {
        $id=substr($buff,0,4);
        $buff=substr($buff,4);

	$id=unpack("i*",$id);
	return $id[1];	
}

function getChar(&$buff) {

	for($i=0;isset($buff{$i})&& $buff{$i} != "\x00";$i++);
        $char=substr($buff,0,$i);
        $buff=substr($buff,$i+1);
	if($i==0) return false;
	return $char;	
}

function errorCheck() {

	global $_RESULT;
	$_RESULT['js'].='document.location="/index.php";';
	unset($_SESSION);
}



function parse() {

global $_RESULT;

if($_REQUEST['call'] == 'login') {
	
	$ret=client_exec_php('USER_LOGIN',$_REQUEST['user']."\x00pass");
	if($ret===false || ($id=getInt($ret)) == 0) {
		$_RESULT['js']='document.getElementById("login_error").innerHTML="<span style=\'color:red\'>ERROR LOGIN&</span>";';
		return 0;
	} else {
		$_SESSION['nick']=$_REQUEST['user'];
		$_SESSION['id']=$id;
	}

	$_RESULT['js'].='document.location="/chat.php";';
}
else if($_REQUEST['call'] == 'list') {

	$ret=client_exec_php('USER_LIST_PLACE',pack("i*",$_SESSION['id']));
	
	if($ret === false) { errorCheck(); return 0;}

	$span='';
	while(($buff=getChar($ret)) != false ) {
		preg_match('/^([^:]+):([^:]+):(.*)$/',$buff,$ret2);
		$span.='<span onclick=\"return join_chat('.$ret2['2'].')\" style=\"cursor:pointer\">'.$ret2['1'].'('.$ret2['3'].')</span><br/>';
	}
	$_RESULT['js'].='document.getElementById("chat_select_div").innerHTML="'.$span.'";';
	
}
else if($_POST['call'] == 'join') {
	$_SESSION['place']=$_POST['id'];
	$ret=client_exec_php('USER_JOIN',pack("i*",$_SESSION['id']).pack("i*",$_SESSION['place']));
	if($ret == false) { errorCheck(); return 0;}

	$_RESULT['js']='document.getElementById("chat").style.display="block";';
	$_RESULT['js'].='setTimeout("reload()", 100);';
	$_RESULT['js'].='document.getElementById("chat_select").style.display="none";';
}
else if($_POST['call'] == 'fresh') {
	
	$ret=client_exec_php('USER_LIST',pack("i*",$_SESSION['id']));
	if($ret === false) { errorCheck(); return 0;}

	while(($buff=getChar($ret)) != false ) {
		preg_match('/^([^:]+):([^:]+):(.*)$/',$buff,$ret2);
		$span.='<span>'.$ret2['1'].' ('.$ret2['2'].'/'.$ret2['3'].')</span><br/>';
	}

	$_RESULT['js']='document.getElementById("chat_user").innerHTML="<b>Nick</b><br/>'.$span.'";';

	$ret=client_exec_php('GET_MESSAGE',pack("i*",$_SESSION['id']));

	//if($ret == false) { errorCheck(); return 0;}

	$_RESULT['result']='';

	while(($span=getChar($ret)) != false) {
		$_RESULT['result'].=$span.'<br/>';
	}
	$_RESULT['js'].='document.getElementById("chat_text").innerHTML+=req.responseJS.result;';


}
else if($_POST['call'] == 'add') {

	$ret=client_exec_php('ADD_MESSAGE',pack("i*",$_SESSION['id']).$_POST['msg']);
	if($ret == false) { errorCheck(); return 0;}

	$_RESULT['result']='';
	while(($span=getChar($ret)) != false) {
		$_RESULT['result'].=$span.'<br/>';
	}
	$_RESULT['js'].='document.getElementById("chat_text").innerHTML+=req.responseJS.result;';
	$_RESULT['js'].='document.getElementById("text").value="";';
	

}


}
parse();
echo '<?xml version="1.0" encoding="windows-1251" ?>
<response>
';

echo do_xml($_RESULT,1);

echo '</response>
';
?>