<?

    $ip = "127.0.0.1";
    $port = 666;
    $magic_key = 'NANA';

$array=array();

$array['MRTG']=0x0001001;
$array['CPS']= 0x0001002;

$array['ADD_MESSAGE']=0x0001F00;
$array['GET_MESSAGE']=0x0001F01;
$array['USER_LOGIN']=0x0001F02;
$array['USER_LOGOUT']=0x0001F03;
$array['USER_LIST']=0x0001F04;
$array['USER_LIST_PLACE']=0x0001F06;
$array['USER_JOIN']=0x0001F05;
$array['USER_STATUS']=0x0001F07;
$array['ACCESS_ERROR']=0xDEADC0FE;
$array['PLACE_ERROR']=0xB0B1BABE;
$array['UPTIME']=0x0000FFF;
$array['EXIT']=0x0001FFF;

function catarg($x,$y) {
	$x=$x<<4;
 return  $x+$y;   
}
function request($code) {
	global $array;
	if(isset($array[$code])) {
		return catarg($array[$code],1);
	} return false;
}

function reply($code) {
	global $array;
	if(isset($array[$code])) {
		return catarg($array[$code],2);
	} return false;
}



function HToN( $val )
{
    $a = unpack("I*",$val);
    return pack("N*",$a[1]);
}
    
function NToH($val )
{
    $a = unpack("N*",$val);
    return pack("I*",$a[1]);
}

?>