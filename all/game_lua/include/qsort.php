<?
$a=array();
for($i=0;$i<25;$i++) {
    $a[$i]=rand(0,25);
}
function qsort(&$a,$l,$r,$d=0) {
    if($l>$r) return;
    $l1=$l;
    $r1=$r;
    $m=(int)(($l+$r)/2);

    while($l<$r)  {
	while (($a[$l]<=$a[$m]) && ($l<$m)) $l++;
        while ($a[$r]>$a[$m])  $r--;
        $t = $a[$l];
        $a[$l] = $a[$r];
        $a[$r] = $t;
        if ($l == $m) $m = $r; 
        else if ($r == $m) $m = $l;
    }
    qsort($a, $l1, $m-1,$d+1); 
    qsort($a, $m+1, $r1,$d+1);


}
echo "Start: ".implode(" ",$a)."\n";
qsort($a,0,count($a));
echo "End: ".implode(" ",$a)."\n";
