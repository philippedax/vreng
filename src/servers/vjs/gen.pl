#! /usr/bin/perl

use strict;

sub usage () {
  print "Usage: gen.pl vjc.hpp objects.cpp package messtype objtype gendir\n";
}

sub gen ($$$$$$\@\%) {
  my ($orig, $gendir, $package, $class, $prefix, $type, $ref1, $ref2) = @_;
  my @const = @{ $ref1 };
  my %strings = %{ $ref2 };

  # Check the output directory
  die "$gendir is not a directory" if ((-e $gendir) && (!-d $gendir));
  if (! -d $gendir) {
    die "Can't create $gendir: $!" unless mkdir($gendir, 0755);
  }

  # Create the intermediate directories
  my $tempdir = $gendir;
  foreach (split /\./, $package) {
    $tempdir .= "/$_";
    next if (-d $tempdir);
    die "$tempdir is not a directory." if (-e $tempdir);
    die "Can't create temp directory $tempdir: $!" unless mkdir($tempdir, 0775);
  }

  # Find out the longest identifier
  my $maxident = 0;
  map {
    if (length($_) > $maxident) { $maxident = length($_); }
  } (@const);

  # Create the output file
  die "Can't create output file $tempdir/$class.java: $!" unless open F, ">$tempdir/".$class.".java";

  print F "package $package;\n";
  print F "\n";
  print F "/**\n";
  print F " * Generated from $orig at ".scalar(localtime())."\n";
  print F " */\n";
  print F "public class $class {\n";
  print F "\n";
  my ($key, $val);
  while ($key = shift(@const)) {
    my $val = shift(@const);
    $val =~ s/^0+//;
    if ((!defined $val) || ($val eq "")) { $val = "0"; }
    print F "\tpublic static final $type ", $prefix, $key;
    print F " "x($maxident-length($key)), " = ", $val, ";\n";
  };
  print F "\n";
  if ((keys %strings) > 0) {
    # Add the string array constants
    my $arr;
    foreach $arr (sort keys %strings) {
      my @vals = @{ $strings{$arr} };
      print F "\tpublic static final String[] $arr = new String[] {\n";
      map {
        print F "\t\t\"$_\",\n";
      } (@vals);
      print F "\t};\n\n";
    }
  }
  print F "}";
  close F;
}

my $vjc = shift(@ARGV);
my $objects = shift(@ARGV);
my $package = shift(@ARGV);
my $mestype = shift(@ARGV);
my $objtype = shift(@ARGV);
my $gendir = shift(@ARGV);

if (!defined $gendir) {
  usage();
  exit(1);
}

my $line;
my @consts;
my %strings;

die "Can't open $vjc : $!" unless open F, $vjc;
while ($line = <F>) {
  if ($line =~ /\#define\s+VJC_(MSG_\S+?)_STRINGS\s+(.*)/) {
    my $name = $1;
    my $val = $2;
    chomp($val);
    my @vals = split(/\s+/, $val);
    @{ $strings{$name} } = @vals;
  } elsif ($line =~ /\#define\s+VJC_(MSG._\S+)\s+(\d+)/) {
    push @consts, $1, $2;
  }
}
close F;

gen($vjc, $gendir, $package, $mestype, "", "int", @consts, %strings);

@consts = ();
%strings = ();

die "Can't open $objects file: $!" unless open F, $objects;
while ($line = <F>) {
  #dax if ($line =~ /\/\*\s*(\d+)\s*\*\/\s*\{\s*(\S+?)_NAME/) {
  if ($line =~ /\/\*\s*(\d+)\s*\*\/\s*\{\s*(\S*)::funcs\}\,/) {
    push @consts, $2, $1;
  }
}
close F;

gen($objects, $gendir, $package, $objtype, "OBJ_", "int", @consts, %strings);
