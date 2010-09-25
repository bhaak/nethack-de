#!/usr/bin/perl -w
use strict;

if (@ARGV < 1)
{
    print "\nUsage:  perl $0 <infile> [de/en/both (default)]\n";
    print "\nReads a file of fortune cookie messages, skipping over empty and\n";
    print "#comment lines, then outputs all messages sorted alphabetically.\n";
    print "Assumes translated messages are followed by a comment (// or #)\n";
    print "containing the English original.\n";
    print "\nOptions:\n";
    print "de   - only keep translated (German) messages\n";
    print "en   - only keep untranslated (English) messages\n";
    print "both - keep both translated (German) and untranslated (English) messages\n";
    print "       (will first print all German, then all English messages)\n";
    print "Whatever the setting, the comments are never included.\n";

    exit;
}

my $infile = shift;
my $part   = shift || "both";

if ($part ne "de" && $part ne "en" && $part ne "both")
{
    print STDERR "Parameter $part not recognized, using 'both'.\n";
    $part = "both";
}

open (INFILE, "<$infile")
    or die "\nFile '$infile' cannot be opened or read. Exit program.\n";

my @german  = ();
my @english = ();
while (my $line = <INFILE>)
{
    next if ($line =~ /^#/ || $line =~ /^\s*$/);

    chomp $line;
    # Line contains a comment -> get German translation.
    # Assumes all translated lines include a comment 
    # (usually just the English text).
    if ($line =~ m#^(.+\S)\s*(//|\#)\s*(\S.+)$#)
    {
        my $german  = $1;
        my $english = $3;

#         print STDERR "$english\n";
        push @german, $german if ($part ne "en");
    }
    # else untranslated
    elsif ($part ne "de" && $line =~ m#^(.+\S)(\s*(//|\#))?$#)
    {
        my $english = $1;
        push @english, $english;
    }
}
close INFILE;

foreach my $rumor (sort @german)
{
    print "$rumor\n";
}

foreach my $rumor (sort @english)
{
    print "$rumor\n";
}
