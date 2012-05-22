<?
function hexdump ($data, $htmloutput = true, $uppercase = false, $return = false)
{
    // Init
    $hexi   = '';
    $ascii  = '';
    $dump   = ($htmloutput === true) ? '<pre>' : '';
    $offset = 0;
    $len    = strlen($data);
 
    // Upper or lower case hexidecimal
    $x = ($uppercase === false) ? 'x' : 'X';
 
    // Iterate string
    for ($i = $j = 0; $i < $len; $i++)
    {
        // Convert to hexidecimal
        $hexi .= sprintf("%02$x ", ord($data[$i]));
 
        // Replace non-viewable bytes with '.'
        if (ord($data[$i]) >= 32) {
            $ascii .= ($htmloutput === true) ?
                            htmlentities($data[$i]) :
                            $data[$i];
        } else {
            $ascii .= '.';
        }
 
        // Add extra column spacing
        if ($j === 7) {
            $hexi  .= ' ';
            $ascii .= ' ';
        }
 
        // Add row
        if (++$j === 16 || $i === $len - 1) {
            // Join the hexi / ascii output
            $dump .= sprintf("%04$x  %-49s  %s", $offset, $hexi, $ascii);
            
            // Reset vars
            $hexi   = $ascii = '';
            $offset += 16;
            $j      = 0;
            
            // Add newline            
            if ($i !== $len - 1) {
                $dump .= "\n";
            }
        }
    }
 
    // Finish dump
    $dump .= $htmloutput === true ?
                '</pre>' :
                '';
    $dump .= "\n";
 
    // Output method
    if ($return === false) {
        echo $dump;
    } else {
        return $dump;
    }
}
    

function send_packet($ip,$port,$packet) {

    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP) or die('socket_create error: '.socket_strerror(socket_last_error()));


    if (socket_set_option($socket, SOL_SOCKET, SO_REUSEADDR, 1) === false) echo "socket_set_option() failed. Reason: ".socket_strerror(socket_last_error($socket));
    socket_connect($socket, $ip,$port);
    socket_send($socket, $packet, strlen($packet), 0);

    $ret=$buff = '';
//	socket_set_nonblock($socket);
    $ret=socket_read($socket,1024,PHP_BINARY_READ);
    socket_close($socket);

    return $ret;
}


function client_exec($code,$msg) {

	global $magic_key,$array,$ip,$port;

$tmp=unpack("h*",$magic_key);
echo "[+] Magic Key 0x".$tmp[1]."\n";
$tmp=unpack("h*",pack("i*",request($code)));
echo "[+] ID ".$code." - ".$tmp[1]." REQUEST\n";
$id=pack("i*",request($code));
$packet = $magic_key.$id.$msg."\x00";

echo "Send packet (".strlen($packet)."):\n--------------------------------------------------------------------------\n";
hexdump($packet,false);
echo "--------------------------------------------------------------------------\n";
$buff=send_packet($ip,$port,$packet);

if($buff) {
	echo "Recevied byte (".strlen($buff).") :\n--------------------------------------------------------------------------\n";
	echo hexdump($buff,false);
	echo "--------------------------------------------------------------------------\n";

	$tmp=unpack("H*",substr($buff,0,4));

	if(substr($buff,0,4) == $magic_key) {
	 echo "[+] Magic Key TRUE - 0x".$tmp[1]."\n";
	} else {
	 echo "[+] Magic Key FALSE - 0x".$tmp[1].'-'.$magic_key."\n";
	}

	$id=substr($buff,4,4);
	$flag=false;
	foreach($array as $code=>$byte) {
	

		if(pack("i*",reply($code)) === $id) {
		        echo "[+] ID ".$code." REPLY \n";
			$flag=true;
			break;
		}
	}
	if($id == pack("i*",0xDEADC0FE)) {
			$id=unpack("H*",$id);
		        echo "[+] ID ACCESS  DENIED ".$id[1]." REPLY \n";

			return false;
	}
	else if($id == pack("i*",0xB0B1BABE)) {
			$id=unpack("H*",$id);
		        echo "[+] ID PLACE DENIED ".$id[1]." REPLY \n";

			return false;
	}
	else if($flag == false) {
			$id=unpack("H*",$id);
		        echo "[+] ID Unknow 0x".$id[1]." REPLY \n";
			return false;
	}


	$buff=substr($buff,8);


	echo "Recevied buffer:\n--------------------------------------------------------------------------\n";
	echo hexdump($buff,false);
	echo "--------------------------------------------------------------------------\n";
	echo "\n\n";


}


}

function client_exec_php($code,$msg) {

	global $_RESULT,$magic_key,$array,$ip,$port;

$tmp=unpack("h*",$magic_key);
$tmp=unpack("h*",pack("i*",request($code)));
$id=pack("i*",request($code));
$packet = $magic_key.$id.$msg."\x00";

$_RESULT['send'][]=hexdump($packet,false,true,true);

$buff=send_packet($ip,$port,$packet);

$_RESULT['recv'][]=hexdump($buff,false,true,true);

	$tmp=unpack("H*",substr($buff,0,4));

	$id=substr($buff,4,4);
	$flag=false;
	foreach($array as $code=>$byte) {
	

		if(pack("i*",reply($code)) === $id) {
			$flag=true;
			break;
		}
	}
	if($id == pack("i*",0xDEADC0FE)) {
			return false;
	}
	else if($id == pack("i*",0xB0B1BABE)) {
			return false;
	}
	else if($flag == false) {
			return false;
	}


	$buff=substr($buff,8);
	if($buff === false) $buff=true;
	return $buff;
}

  
?>