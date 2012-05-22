package antiddos;
use nginx;
use strict;
use warnings;
use IPC::SysV qw(IPC_CREAT ftok IPC_PRIVATE);

my $key = ftok("/tmp",'A');
my $id =  msgget($key,1) || msgget($key,IPC_CREAT|0666);
my $ret = 0;
sub handler {
    my $r = shift;
    $ret = msgsnd($id, $r->remote_addr()."\t".$r->uri()."\t".$r->header_in('Referer')."\t".$r->header_in('User-Agent'), 0);
    return DECLINED;
}

1;
__END__
