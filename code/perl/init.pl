#!/usr/bin/perl
use FCGI;
use CGI;
use Cwd 'abs_path';
use File::stat;
use POSIX;
use Template;
use strict;
use warnings;

die ('need root priviligies to run') if ($> != 0);

my $sock='/tmp/perl.sock';
my $me=abs_path($0);
my $mpf=$me;
$mpf =~ s/.pl$/.pid/;

if ( -f $mpf ) {
    open F,$mpf;
    my $p=<F>;
    chomp $p;
    kill 9,$p;
    undef $p;
    unlink($sock) if ( -f $sock );
    close F;
}

sub error500 {
    my $msg=shift;
    print "Status: 500\nContent-Type:text/plain\n\n$msg\n";
}


sub error404 {
    my $msg=shift;
    print "Status: 404\nContent-Type:text/plain\n\nFile $msg, not found\n";
}


if(fork() != 0) { exit();}

open F,">$mpf";
print F $$;
close F;

$0="Init.pl :: /tmp/perl.sock :: sleep";
my ($host,$req,$db,$file,$path,$cache,$time,$code,$initfile,$tpl,$site);
my %sites=();
$req = FCGI::Request(\*STDIN,\*STDOUT,\*STDERR,\%ENV,FCGI::OpenSocket( $sock, 256));
chmod(0777,$sock);


sub loadfile {

    if( ! defined $site->[1]{$file} or $site->[1]{$file}[0]+5-1 < $time) {
	if( ! -f $path ) { error404($file); return 0;}

	$cache=\@{$site->[1]{$file}};
	my $mtime=(stat($path))->mtime;

	if ( ! defined $site->[1]{$file} or !defined $cache->[1] or $cache->[1] < $mtime) {
	    $cache->[1]=$mtime;
	    open F,$path;
	    $code="";
	    while(<F>) {
		$code=$code.$_;
	    }
	    $code=eval ("sub {\n".$code."\n}");
	    close(F);
	    if(!$code) { error500('Unable to eval '.$file."\nError:".$@);return 0; }
	    $cache->[2]=$code;
	    print "PerlCache: now()\n";
	} else {
	    print "PerlCache: check()\n";
	}

	$cache->[0]=$time;
    } else {
	$cache=\@{$site->[1]{$file}};
        print "PerlCache: ".($cache->[0] + 5 - $time)."\n";
    }
    return $cache;
}

sub loadinit {
    if( ! defined $site->[1]{"__init.pl"} or $site->[1]{"__init.pl"}[0]+5 < $time) {
	$site->[1]{"__init.pl"}[0]=$time;
	if( ! -f $initfile ) { 
	    return 0;
	}

	my ($mtime)=(stat($initfile))->mtime;

	if ( ! defined $site->[1]{"__init.pl"} or ! $site->[1]{"__init.pl"}[1] or $site->[1]{"__init.pl"}[1]  != $mtime) {
	    $site->[1]{"__init.pl"}[1]=$mtime;
	    open F,$initfile;
	    $code="";
	    while(<F>) {
		$code=$code.$_;
	    }
	    $code=eval ("sub {".$code."\n}");
	    close(F);
	    if(!$code) { error500('Unable to eval '.$file."\nError:".$@);return 1; }
	    $site->[2]=$code->();
	}

    }
    return 0;
}

sub parse {

    $cache = 0;
    $host = lc($ENV{"HTTP_HOST"});
    $host =~ s/^www.//;
    $path = "/www/".$host."/www".$ENV{"SCRIPT_NAME"};
    $file = $ENV{"SCRIPT_NAME"};
    $initfile="/www/".$host."/init.pl";

    $0="Init.pl :: /tmp/perl.sock :: parse $host$file";
    if (! defined $sites{$host}) {
	$sites{$host} = [];
	$sites{$host}[0] = Template->new({INCLUDE_PATH => '/www/'.$host.'/tpl/',COMPILE_DIR => '/www/engine/tpl_c/',STAT_TTL=>6}) or return 0;
	$sites{$host}[1] = ();
    }
    $site=\@{$sites{$host}};
    $db=$site->[2];
    $tpl=$sites{$host}[0];

    return 1 if(loadinit());
    $cache = loadfile();
    
    if(!$cache or !$cache->[2]) {
	error500('Broken code in '.$file);
	return 0;
    }

    eval { $cache->[2]->() } or print ($@);
    return 1;
}
my $uid=getpwnam('ngx');
setuid($uid);
while( $req->Accept() >= 0 ){
    $time=time();
    parse();
    $0="Init.pl :: /tmp/perl.sock :: sleep ";

    $req->Flush;
    $req->Finish;
}
