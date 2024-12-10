#! /usr/bin/perl

use Time::Local;

if (@ARGV != 2){
    # input station name and plot date
    print "Usage : \$pipe_low.pl yyyymmdd yyyymmdd\n"; 
    exit;
}else{
    $date1 = $ARGV[0];
    $date2 = $ARGV[1];
}

print substr($string, 0, 6);
$dt  = timelocal(0,0,0,substr($date1,6,2),substr($date1,4,2)-1,substr($date1,0,4)-1900);
$dte = timelocal(0,0,0,substr($date2,6,2),substr($date2,4,2)-1,substr($date2,0,4)-1900);

while($dt <= $dte){

    ($mday, $mon, $year) = (localtime $dt)[3,4,5];
    $data_dt = sprintf("%04d%02d%02d",$year+1900,$mon+1,$mday);
    print $data_dt."\n";
    system ("./AqDS ".$data_dt);
    system ("./AqDS_Chk_Sat ".$data_dt);
    system ("./AqDS_open_everyday ".$data_dt);

    $dt = $dt + 60*60*24;

}
