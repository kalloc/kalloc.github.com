<?php
include('var.php');
include('inc.php');



//client_exec('USER_LOGIN',"n1ckname\x00pass");
//client_exec('USER_JOIN',pack("i*",1).pack("i*",1));
client_exec('USER_LOGIN',"bla\x00blabla");
client_exec('USER_STATUS',pack("i*",1).pack("i*",1));
//client_exec('USER_JOIN',pack("i*",1).pack("i*",1));
client_exec("USER_LIST_PLACE",pack("i*",1));
/*client_exec("USER_LIST",pack("i*",1));
client_exec('USER_LOGOUT',pack("i*",1)."n1ckname");
*/
//client_exec('UPTIME',pack("i*",1));


?>