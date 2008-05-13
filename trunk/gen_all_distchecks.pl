#!/usr/bin/perl

# 
# Generate a script that executes distcheck for all the possible configurations.
# 
# 
# Usage: ./gen_all_distchecks.pl > distcheck_all.sh
# 
# $Id$
#


use strict;

# List of configure options.
my @CONFIGURE_PARAMS=("--enable-cli", "--enable-www", "--enable-debug", "--enable-unittest", "--enable-session-saving", "--enable-event-callback", " --enable-url");

my @PERMANENT_CONFIGURE_PARAMS=@ARGV;

sub permute (@)
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

my $counter = 0;
my $g_counter = 0;

print "echo \"distchecking " . scalar(@sets) . " (" . scalar(@sets)*scalar(@sets) . ") BTG configurations\" > distcheck.log\n";

foreach ( @sets ) 
{
    print "# $counter:\n";
    print "echo \"#$counter: Configure @$_\" && \\
echo \"BTG config #$counter args: @$_\" >> distcheck.log && \\
./configure @$_ &> distcheck$counter-configure.log && \\
echo \"#$counter: make clean\" && \\
make clean &> distcheck$counter-make_clean.log
";
    my $dc_counter = 0;
    foreach ( @sets )
    {
        print "# general config $g_counter:\n";
        print "# distcheck $dc_counter:\n";
        print "echo \"#$dc_counter: make distcheck DISTCHECK_CONFIGURE_FLAGS=\\\"@$_\\\"\" && \\
echo \"BTG config #$counter distcheck$dc_counter\" >> distcheck.log && \\
make distcheck DISTCHECK_CONFIGURE_FLAGS=\"@$_\" &> distcheck$counter-make_distcheck$dc_counter.log && \\
echo \"#$counter: make distcheck done\"
";
        $dc_counter++;
	$g_counter++;
    }

    $counter++;
} 
