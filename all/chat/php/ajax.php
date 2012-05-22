<?
header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Pragma: no-cache');
header('Content-Type: text/xml;charset=windows-1251');
include('var.php');
include('inc.php');


define('DELIM',"  ");
function do_xml($ptr,$del=0) {
	$ret='';
	if(is_array($ptr)) { 
          	 foreach($ptr as $key=>$val)  {
			$i=0;
			while(isset($val[$i]) && is_array($val)) {
				$ret.=str_repeat(DELIM,$del).'<'.$key.'>'.(is_array($val[$i]) ? "\n".do_xml($val[$i],$del+1).str_repeat(DELIM,$del) : '<![CDATA['.$val[$i]."]]>").'</'.$key.'>'."\n";
				unset($val[$i]);
				$i++;
			}
			if(!empty($val) || is_numeric($val))
				$ret.=str_repeat(DELIM,$del).'<'.$key.'>'.(is_array($val) ? "\n".do_xml($val,$del+1).str_repeat(DELIM,$del) : '<![CDATA['.$val."]]>").'</'.$key.'>'."\n";
		 }
	}
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
	$_RESULT['action']['reload']='/index.php';
	unset($_SESSION);
}


function parse() {

global $_RESULT;
session_start();
if(!isset($_SESSION['onlive']))  return ($_RESULT['action']['reload']='/session.php?redir='.$_SERVER['REQUEST_URI']);

if($_REQUEST['call'] == 'login') {
	
	$ret=client_exec_php('USER_LOGIN',$_REQUEST['user']."\x00pass");
	if($ret===false || ($id=getInt($ret)) == 0) {
		$_RESULT['action']['error']='login';
		return 0;           
	} else {
		$_SESSION['nick']=$_REQUEST['user'];
		$_SESSION['id']=$id;
	}

	$_RESULT['action']['reload']='/chat.php';
}
else if($_REQUEST['call'] == 'status') {
	$ret=client_exec_php('USER_STATUS',pack("i*",$_SESSION['id']));
	if($ret===false) {
		$_RESULT['action']['error']='login';
		$_RESULT['action']['reload']='/index.php';
		return 0;           
	} else {
		$id=getInt($ret);
		if($id > 0) {
			$_RESULT['action']='join';
		} else {

			$ret=client_exec_php('USER_LIST_PLACE',pack("i*",$_SESSION['id']));
			if($ret === false) { errorCheck(); return 0;}
			$span='';
			while(($buff=getChar($ret)) != false ) {
				preg_match('/^([^:]+):([^:]+):([0-9]+)$/',$buff,$ret2);
				$_RESULT['chat']['place'][]=array('name'=>$ret2['1'],'id'=>$ret2['2'],'user'=>$ret2['3']);
			}
		}
	}
}
else if($_REQUEST['call'] == 'list') {

	$ret=client_exec_php('USER_LIST_PLACE',pack("i*",$_SESSION['id']));
	
	if($ret === false) { errorCheck(); return 0;}

	$span='';
	while(($buff=getChar($ret)) != false ) {
		preg_match('/^([^:]+):([^:]+):([0-9]+)$/',$buff,$ret2);
		$_RESULT['chat']['place'][]=array('name'=>$ret2['1'],'id'=>$ret2['2'],'user'=>$ret2['3']);
	}
	
}
else if($_POST['call'] == 'join') {
	$_SESSION['place']=$_POST['id'];
	$ret=client_exec_php('USER_JOIN',pack("i*",$_SESSION['id']).pack("i*",$_SESSION['place']));
	if($ret == false) { errorCheck(); return 0;}
	$_RESULT['action']='join';
}
else if($_POST['call'] == 'fresh') {
	
	$ret=client_exec_php('USER_LIST',pack("i*",$_SESSION['id']));
	if($ret === false) { errorCheck(); return 0;}

	while(($buff=getChar($ret)) != false ) {
		preg_match('/^([^:]+):([^:]+):(.*)$/',$buff,$ret2);
		$_RESULT['chat']['user'][]=array('name'=>$ret2['1'],'num'=>$ret2['2'],'ping'=>$ret2['3']);
	}

	$ret=client_exec_php('GET_MESSAGE',pack("i*",$_SESSION['id']));

	while(($span=getChar($ret)) != false) {
		$_RESULT['chat']['msg'][]=$span;
	}

}
else if($_POST['call'] == 'add') {

	$ret=client_exec_php('ADD_MESSAGE',pack("i*",$_SESSION['id']).$_POST['msg']);
	if($ret == false) { errorCheck(); return 0;}

	while(($span=getChar($ret)) != false) {
		$_RESULT['chat']['msg'][]=$span;
	}
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