#! /usr/bin/perl

#
# Launch generated build/distcheck failes in parallel
#
#
# $Id$
#

use strict;
use warnings;
use POSIX qw(:sys_wait_h); # waitpid, WNOHANG

my $usage = "USAGE: $0 <dir> [forks_cnt]";

my $dir = shift or die "No directory specified. $usage";
my $forks = shift || 10;
my $shell = "/bin/bash";

my $d;
opendir $d, $dir or die "Can't open dir \"$dir\" ($!)";

my %pids;

my $sigusr1flag = 0;
$SIG{USR1} = sub { $sigusr1flag = 1; };

sub waitpids
{
	my $wpid = waitpid(-1,WNOHANG);
	if (exists $pids{$wpid})
	{
		if ($? == 0)
		{
			my $F;
			open $F,">$dir/$pids{$wpid}{dir}.ok" or die "Can't open file \"$dir/$pids{$wpid}{dir}.ok\" ($!)";
			close $F;
		}
		else
		{
			my $F;
			open $F,">$dir/$pids{$wpid}{dir}.fail" or die "Can't open file \"$dir/$pids{$wpid}{dir}.fail\" ($!)";
			print $F $?;
			close $F;
		}
		print STDERR "Completed: $pids{$wpid}{dir} (pid = $wpid), " . ( $? ? "error: $?" : "ok" ) . "\n";
		delete $pids{$wpid};
	}
}

while(my $dentry = readdir $d)
{
	next if $dentry eq ".";
	next if $dentry eq "..";
	
	while(scalar keys %pids >= $forks)
	{
		waitpids();
		sleep 1;
	}
	
	$sigusr1flag = 0;
	my $pid = fork();
	unless(defined $pid)
	{
		die "Can't create process: $!";
	}
	if ($pid == 0)
	{
		chdir "$dir/$dentry" or die "Can't chdir to \"$dir/$dentry\" ($!)";

		kill 'SIGUSR1', getppid(); # tell parent that we're launched

		while ( !$sigusr1flag ) # wait for SIGUSR1
		{
			sleep 1;
		}

		close STDIN;
		open STDIN,"</dev/null" or die "Can't reopen STDIN to /dev/null ($!)";
		close STDOUT;
		open STDOUT,">../$dentry.log" or die "Can't open $dentry.log ($!)";
		close STDERR;
		open STDERR,">../$dentry.log";

		exec $shell,"./.launch";
		die "Can't exec: $!";
	}

	while ( !$sigusr1flag )
	{
		print STDERR "waiting for $dentry ($pid) to launch\n";
		my $wpid = waitpid($pid, WNOHANG);
		if ($wpid > 0)
		{
			die "Process $dentry ($pid) died!";
		}
		sleep 1;
	}

	$pids{$pid}{dir} = $dentry;
	print STDERR "Launched for $pids{$pid}{dir} ($pid)\n";
	
	kill 'SIGUSR1', $pid; # launch it
}

while(scalar keys %pids)
{
	waitpids();
	sleep 1;
}
