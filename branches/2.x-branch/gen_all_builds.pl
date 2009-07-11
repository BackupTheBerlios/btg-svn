#!/usr/bin/perl

# 
# Generate a script that builts all the possible configurations.
# 
# 
# Usage: ./gen_all_builds.pl > build_all.sh
# 
# Execute build_all.sh, which will fail if one of the configurations
# fail to compile.
#
# $Id$
#


use strict;
use warnings;
use Cwd qw(getcwd);

# List of configure options.
my @CONFIGURE_PARAMS=("--enable-cli", "--enable-www", "--enable-debug", "--enable-unittest", "--enable-session-saving", "--enable-event-callback", " --enable-url", "--enable-viewer");

my $mode = $ARGV[0] || "";
my $dir = "";
my $curdir = getcwd(); # the dir with configure script
if ($mode eq "help" || $mode eq "-h")
{
	print "USAGE: $0 [build|distcheck|distcheck_short|doxygen_launch <dir>|build_parallel <dir>|distcheck_parallel <dir>|distcheck_short_parallel <dir>|check_parallel <dir>] [permanent configure params ...]\n";
	exit 1;
}
elsif ($mode eq "build" || $mode eq "distcheck" || $mode eq "distcheck_short")
{
	shift;
}
elsif ($mode eq "doxygen_launch" || $mode eq "build_parallel" || $mode eq "distcheck_parallel" || $mode eq "distcheck_short_parallel" || $mode eq "check_parallel")
{
	shift;
	$dir = shift or die "You have to set directory. See usage.";
	print STDERR "directory \"$dir\" is like configure parameter!\n" if $dir =~ /^\-\-/;
	mkdir $dir or die "Can't create dir \"$dir\"";
}
else
{
	# default
	$mode = "build";
}

my @PERMANENT_CONFIGURE_PARAMS=@ARGV;

sub permute
{
    if ( @_ <= 1 ) 
	{
            return [ @_ ];
        } else 
	{
            my @out;
            for( my $i = 0 ; $i < @_ ; $i++ ) 
	    {
                my @tmp = @_;
                my $ith = splice @tmp, $i, 1;
                push @out, map { [ $ith, @{$_} ] } permute( @tmp );
            }
            return @out;
        }
}

sub subsets 
{
    my @words = @_;
    my $n = @words;
    my @sets;
    my @scored;
    my %seen;

    # create "binary" sets, '1's represent words present
    push @sets, sprintf "%0${n}b", $_ for 0 .. 2**$n-1;

    # $x is for unique sort keys ("scores")
    my $x = $n - 1;

    for my $si ( 0 .. $#sets ) 
    { # need $si in score
        my $set = $sets[ $si ];
	
        # split into groups of adjacent terms
        my @groups = split( /0/, $set);
	
        my $score = 0;
        for my $gi ( 0 .. $#groups ) 
	{ # need $gi in score
            my $group = $groups[ $gi ];
            next unless $group;
	    
            # sets are scored by length, number,
            # and lefthandedness of their groups
            my $len = length $group;
            $score += $x**(2*$len) + ($x-$gi)*$len + $si;
        }

        # convert "binary" sets to sets of words
        my @wordset;
        my $i = 0;
        for( split //, $set) 
	{
            my $word = $words[$i++];
            $_ && push @wordset, $word;
        }
        push @scored, [$score, [@wordset]];
	
        $seen{$score}++; # to prove our scores are unique
    }
    
    # for ( sort keys %seen ) { die "Dupe: $_" if $seen{$_}>1 }

    map  { $_->[1] }
    sort { $b->[0] <=> $a->[0] }
    @scored;

} 

my @sets = subsets( @CONFIGURE_PARAMS );

push @$_ => @PERMANENT_CONFIGURE_PARAMS foreach @sets;

if ($mode eq "build")
{
	my $counter = 0;

	print "echo \"Building " . scalar(@sets) . " BTG configurations\" >.build.log\n";

	foreach ( @sets ) 
	{
		print "
# $counter:
echo \"#$counter: configure @$_\" && \\
echo \"BTG config #$counter args: @$_\" >>.build.log && \\
./configure @$_ >.build$counter-configure.log 2>.build$counter-configure.err && \\
echo \"#$counter: make\" && \\
make >.build$counter-make.log 2>.build$counter-make.err && \\
echo \"BTG config #$counter built.\" >>.build.log && \\
echo \"#$counter: make done\"
";
		$counter++;
	}
}
elsif ($mode eq "distcheck")
{
	my $counter = 0;
	my $g_counter = 0;

	print "echo \"distchecking " . scalar(@sets) . " (" . scalar(@sets)*scalar(@sets) . ") BTG configurations\" >.distcheck.log\n";

	foreach ( @sets ) 
	{
		print "
#
# $counter:
echo \"#$counter: configure @$_\" && \\
echo \"BTG config #$counter args: @$_\" >>.distcheck.log && \\
./configure @$_ >.distcheck$counter-configure.log 2>.distcheck$counter-configure.err
";
		my $dc_counter = 0;
		foreach ( @sets )
		{
			print "
# general config $g_counter:
# distcheck $dc_counter:
echo \"#$dc_counter: make distcheck DISTCHECK_CONFIGURE_FLAGS=\\\"@$_\\\"\" && \\
echo \"BTG config #$counter distcheck$dc_counter\" >>.distcheck.log && \\
make distcheck DISTCHECK_CONFIGURE_FLAGS=\"@$_\" >.distcheck$counter-make_distcheck$dc_counter.log 2>.distcheck$counter-make_distcheck$dc_counter.err && \\
echo \"#$dc_counter: make distcheck done\"
";
			$dc_counter++;
			$g_counter++;
		}
		$counter++;
	}
} 
elsif ($mode eq "distcheck_short")
{
	my $counter = 0;

	print "echo \"distchecking " . scalar(@sets) . " BTG configurations\" >.distcheck_short.log\n";

	foreach ( @sets ) 
	{
		print "
# $counter:
echo \"#$counter: configure @$_\" && \\
echo \"BTG config #$counter args: @$_\" >>.distcheck_short.log && \\
./configure @$_ >.distcheck_short$counter-configure.log 2>.distcheck_short$counter-configure.err
echo \"#$counter: make distcheck DISTCHECK_CONFIGURE_FLAGS=\\\"@$_\\\"\" && \\
echo \"BTG config #$counter distcheck$counter\" >>.distcheck_short.log && \\
make distcheck DISTCHECK_CONFIGURE_FLAGS=\"@$_\" >.distcheck_short$counter-make_distcheck.log 2>.distcheck_short$counter-make_distcheck.err && \\
echo \"#$counter: make distcheck done\"
";
		$counter++;
	}
} 
elsif ($mode eq "build_parallel")
{
	my $counter = 0;

	foreach ( @sets ) 
	{
		mkdir "$dir/build$counter" or die "Can't create dir \"$dir/build$counter\" ($!)";
		
		my $f;
		open $f,">$dir/build$counter/.launch" or die "Can't create file \"$dir/build$counter/.launch\" ($!)";
		print $f "
# $counter:
echo \"#$counter: configure @$_\" && \\
$curdir/configure @$_ >.build-configure.log 2>.build-configure.err && \\
echo \"#$counter: configure done\"

echo \"#$counter: make\" && \\
make >.build-make.log 2>.build-make.err && \\
echo \"#$counter: make done\" && \\
echo \"#$counter: make clean\" && \\
make clean >.build-make_clean.log 2>.build-make_clean.err && \\
echo \"#$counter: make clean done\" && \\
echo \"#$counter: make distclean\" && \\
make distclean >.build-make_distclean.log 2>.build-make_distclean.err && \\
echo \"#$counter: make distclean done\"
retval=\$?

# freeing disk space, or we can run out of it
ls -l >.build-ll.log 2>.build-ll.err
chmod -R u+rwx *
rm -Rf * # should not touch newly created .build-* files

exit \$retval
";
		close $f;
		$counter++;
	}
}
elsif ($mode eq "doxygen_launch")
{
		mkdir "$dir/doxygen" or die "Can't create dir \"$dir/doxygen\" ($!)";

		my $f;
		open $f,">$dir/doxygen/.launch" or die "Can't create file \"$dir/doxygen/.launch\" ($!)";

		print $f "
echo configure @PERMANENT_CONFIGURE_PARAMS && \\
$curdir/configure @PERMANENT_CONFIGURE_PARAMS >.doxygen-configure.log 2>.doxygen-configure.err && \\
echo configure done

echo make doxygen && \\
make doxygen >.doxygen-make_doxygen.log 2>.doxygen-make_doxygen.err && \\
echo make doxygen done
retval=\$?

# freeing disk space
#ls -l >.doxygen-ll.log 2>.doxygen-ll.err
chmod -R u+rwx *
rm -Rf * # should not touch newly created .doxygen-* files

exit \$retval
";
			close $f;
}
elsif ($mode eq "distcheck_parallel")
{
	my $counter = 0;
	my $g_counter = 0;

	foreach ( @sets ) 
	{
		my $dc_counter = 0;
		foreach ( @sets )
		{
			mkdir "$dir/distcheck$counter-$dc_counter" or die "Can't create dir \"$dir/distcheck$counter-$dc_counter\" ($!)";

			my $f;
			open $f,">$dir/distcheck$counter-$dc_counter/.launch" or die "Can't create file \"$dir/distcheck$counter-$dc_counter/.launch\" ($!)";

			print $f "
# general config $g_counter:

# configure $counter:
echo \"#$counter: configure @$_\" && \\
$curdir/configure @$_ >.distcheck-configure.log 2>.distcheck-configure.err && \\
echo \"#$counter: configure done\"

# distcheck $dc_counter:
echo \"#$dc_counter: make distcheck DISTCHECK_CONFIGURE_FLAGS=\\\"@$_\\\"\" && \\
make distcheck DISTCHECK_CONFIGURE_FLAGS=\"@$_\" >.distcheck-make_distcheck.log 2>.distcheck-make_distcheck.err && \\
echo \"#$dc_counter: make distcheck done\" && \\
echo \"#$counter: make clean\" && \\
make clean >.distcheck-make_clean.log 2>.distcheck-make_clean.err && \\
echo \"#$counter: make clean done\" && \\
echo \"#$counter: make distclean\" && \\
make distclean >.distcheck-make_distclean.log 2>.distcheck-make_distclean.err && \\
echo \"#$counter: make distclean done\"
retval=\$?

# freeing disk space, or we can run out of it
ls -l >.distcheck-ll.log 2>.distcheck-ll.err
chmod -R u+rwx *
rm -Rf * # should not touch newly created .distcheck-* files

exit \$retval
";
			close $f;
			
			$dc_counter++;
			$g_counter++;
		}
		$counter++;
	}
} 
elsif ($mode eq "distcheck_short_parallel")
{
	my $counter = 0;

	foreach ( @sets ) 
	{
		mkdir "$dir/distcheck_short$counter" or die "Can't create dir \"$dir/distcheck_short$counter\" ($!)";

		my $f;
		open $f,">$dir/distcheck_short$counter/.launch" or die "Can't create file \"$dir/distcheck_short$counter/.launch\" ($!)";

		print $f "
# $counter:
echo \"#$counter: configure @$_\" && \\
$curdir/configure @$_ >.distcheck_short-configure.log 2>.distcheck_short-configure.err && \\
echo \"#$counter: configure done\"

echo \"#$counter: make distcheck DISTCHECK_CONFIGURE_FLAGS=\\\"@$_\\\"\" && \\
make distcheck DISTCHECK_CONFIGURE_FLAGS=\"@$_\" >.distcheck_short-make_distcheck.log 2>.distcheck_short-make_distcheck.err && \\
echo \"#$counter: make distcheck done\" && \\
echo \"#$counter: make clean\" && \\
make clean >.distcheck_short-make_clean.log 2>.distcheck_short-make_clean.err && \\
echo \"#$counter: make clean done\" && \\
echo \"#$counter: make distclean\" && \\
make distclean >.distcheck_short-make_distclean.log 2>.distcheck_short-make_distclean.err && \\
echo \"#$counter: make distclean done\"
retval=\$?

# freeing disk space, or we can run out of it
ls -l >.distcheck_short-ll.log 2>.distcheck_short-ll.err
chmod -R u+rwx *
rm -Rf * # should not touch newly created .distcheck_short-* files

exit \$retval
";
		close $f;		
		$counter++;
	}
} 
elsif ($mode eq "check_parallel")
{
	my $counter = 0;

	foreach ( @sets ) 
	{
		mkdir "$dir/check$counter" or die "Can't create dir \"$dir/check$counter\" ($!)";

		my $f;
		open $f,">$dir/check$counter/.launch" or die "Can't create file \"$dir/check$counter/.launch\" ($!)";

		print $f "
# $counter:
echo \"#$counter: configure @$_\" && \\
$curdir/configure @$_ >.check-configure.log 2>.check-configure.err && \\
echo \"#$counter: configure done\"

echo \"#$counter: make check\" && \\
make check >.check-make_check.log 2>.check-make_check.err && \\
echo \"#$counter: make check done\" && \\
echo \"#$counter: make clean\" && \\
make clean >.check-make_clean.log 2>.check-make_clean.err && \\
echo \"#$counter: make clean done\" && \\
echo \"#$counter: make distclean\" && \\
make distclean >.check-make_distclean.log 2>.check-make_distclean.err && \\
echo \"#$counter: make distclean done\"
retval=\$?

# freeing disk space, or we can run out of it
ls -l >.check-ll.log 2>.check-ll.err
chmod -R u+rwx *
rm -Rf * # should not touch newly created .check-* files

exit \$retval
";
		close $f;		
		$counter++;
	}
} 
