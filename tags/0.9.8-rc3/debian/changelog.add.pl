#! /usr/bin/perl -w

use strict;

my $usage = "USAGE: $0 <version> [suffix]";

my $version = shift or die $usage;
my $suffix = shift || "";

open N, ">", "changelog.new" or die "Can't open changelog.new for writing";
open O, "<", "changelog" or die "Can't open changelog";
my @old_changelog = <O>;
close O;

print N "btg$suffix ($version) " . ($suffix?"unstable":"stable") . "; urgency=low\n\n";

my $begin = 0;
my $space = 0;
open F, "<", "../ChangeLog" or die "Can't open ../ChangeLog";
while(<F>)
{
	if (!$begin && /\-\-\-\-\-\-\-\-\-\-/)
	{
		$begin = 1;
		next;
	}
	if (!$space && /^\s*$/)
	{
		$space = 1;
		next;
	}
	next unless $begin;
	last if /^\s*$/;
	if ($space)
	{
		my $line = $_;		
		print N "    $_" unless grep { index($_,$line) != -1 } @old_changelog;
	}
	else
	{
		print N "  * $_";
	}
}
close F;

print N "\n";
print N " -- Roman Rybalko <btg\@romanr.info>  ",`date -R`, "\n";
print N foreach @old_changelog;

close N;

rename "changelog", "changelog.old" or die "Can't nename changelog -> changelog.old";
rename "changelog.new", "changelog" or die "Can't nename changelog.new -> changelog";
