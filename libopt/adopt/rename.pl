#!/usr/bin/perl -w

use strict;

my $tests = 0;
my $out = '.';
my($prefix, $filename, $out_tests, $inline, $include, $tests_name, $no_usage);

my $usage = "usage: $0 [--out=<dir>] [--filename=<name>] [--out-tests=<dir>] [--with-tests=<testname>] [--without-usage] [--include=<include>] [--inline=<inline_func>] <prefix>\n";

sub die_usage() { die $usage; }


while (my $arg = shift @ARGV) {
	if ($arg =~ /^--with-tests/) {
		$tests = 1;

		if ($arg =~ s/^--with-tests=//) {
			$tests_name = $arg;
		}
	}
	elsif ($arg =~ s/^--out=//) {
		$out = $arg;
	}
	elsif ($arg =~ s/^--filename=//) {
		$filename = $arg;
	}
	elsif ($arg =~ s/^--out-tests=//) {
		$out_tests = $arg;
	}
	elsif ($arg =~ s/^--include=//) {
		$include = $arg;
	}
	elsif ($arg =~ s/^--inline=//) {
		$inline = $arg;
	}
	elsif ($arg !~ /^--/ && ! $prefix) {
		$prefix = $arg;
	}
	elsif ($arg eq '--without-usage') {
		$no_usage = 1;
	}
	elsif ($arg eq '--help') {
		print STDOUT $usage;
		exit;
	}
	else {
		print STDERR "$0: unknown argument: $arg\n";
		die_usage();
	}
}

die_usage() unless $prefix;

$filename = $prefix unless($filename);

my $filename_upper = $filename;
$filename_upper =~ tr/a-z/A-Z/;

my $prefix_upper = $prefix;
$prefix_upper =~ tr/a-z/A-Z/;

translate("adopt.c", "${out}/${filename}.c");
translate("adopt.h", "${out}/${filename}.h");

if ($tests)
{
	$out_tests = $out unless($out_tests);

	if ($tests_name) {
		$tests_name =~ s/::/_/g;
	} else {
		$tests_name = $prefix;
	}

	my $tests_filename = $tests_name;
	$tests_filename =~ s/.*_//;

	translate("tests/adopt.c", "${out_tests}/${tests_filename}.c");
}

sub translate {
	my($in, $out) = @_;

	open(IN, $in) || die "$0: could not open ${in}: $!\n";
	my $contents = join('', <IN>);
	close(IN);

	$contents =~ s/\n\/\*\*\n( \*[^\n]*\n)* \*\/\nint adopt_usage_fprint\(.*?\);\n//s
	 if ($no_usage);
	$contents =~ s/\nint adopt_usage_fprint.*}\n//s
	 if ($no_usage);

	$contents =~ s/test_adopt__/test_${filename}__/g;

	# if a prefix becomes foo_opt, we want to rewrite adopt_opt specially
	# to avoid it becoming foo_opt_opt
	$contents =~ s/adopt_opt/${prefix}/g if ($prefix =~ /_opt$/);

	$contents =~ s/ifndef ADOPT_H/ifndef ${filename_upper}_H/g;
	$contents =~ s/define ADOPT_H/define ${filename_upper}_H/g;
	$contents =~ s/endif \/\* ADOPT_H/endif \/* ${filename_upper}_H/g;
	$contents =~ s/adopt\.h/${filename}\.h/g;

	$contents =~ s/adopt_/${prefix}_/g;
	$contents =~ s/ADOPT_/${prefix_upper}_/g;

	if ($include) {
		$contents =~ s/^(#include "opt.h")$/#include "${include}"\n$1/mg;
	}

	if ($inline) {
		$contents =~ s/^INLINE/${inline}/mg;
		$contents =~ s/\n#ifdef _MSC_VER\n.*\n#endif\n//sg;
	}

	if ($tests) {
		$contents =~ s/test_adopt__/test_${tests_name}__/g;
	}

	open(OUT, '>' . $out) || die "$0: could not open ${out}: $!\n";
	print OUT $contents;
	close(OUT);
}

