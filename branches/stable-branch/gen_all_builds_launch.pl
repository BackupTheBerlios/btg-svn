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

sub waitpids
{
	my $wpid = waitpid(-1,WNOHANG);
	if ($wpid > 0)
	{
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
			print STDERR "Completed: $pids{$wpid}{dir} ($wpid), " . ( $? ? "error: $?" : "ok" ) . "\n";
			delete $pids{$wpid};
		}
		else
		{
			print STDERR "unregistered pid = $wpid terminated, " . ( $? ? "error: $?" : "ok" ) . "\n";
		}
	}
}

while(my $dentry = readdir $d)
{
	next if $dentry eq ".";
	next if $dentry eq "..";
	next unless -d "$dir/$dentry";
	
	while(scalar keys %pids >= $forks)
	{
		sleep 1;
		waitpids();
	}
	
	my $pid = fork();
	unless(defined $pid)
	{
		die "Can't create process: $!";
	}
	if ($pid == 0)
	{
		chdir "$dir/$dentry" or die "Can't chdir to \"$dir/$dentry\" ($!)";

		close STDIN;
		open STDIN,"</dev/null" or die "Can't open /dev/null as STDIN ($!)";
		close STDOUT;
		open STDOUT,">../$dentry.log" or die "Can't open $dentry.log as STDOUT ($!)";
		close STDERR;
		open STDERR,">../$dentry.log" or die "Can't open $dentry.log as STDERR ($!)";

		exec $shell,"./.launch";

		die "Can't exec \"$dentry/.launch\": $!";
	}

	$pids{$pid}{dir} = $dentry;
	print STDERR "Launched for $pids{$pid}{dir} ($pid)\n";
}

while(scalar keys %pids)
{
	sleep 1;
	waitpids();
}
