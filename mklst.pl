#! /usr/bin/perl

open(S, "a.adr") || die;
while (<S>) {
	($adr, $name) = split;
	$adr =~ tr/A-Z/a-z/;
	$sym{$adr} = $name;
}
close S;
open(S, "a.dis") || die;
while (<S>) {
	next unless /^[\da-f]{4}/;
	print "$sym{$&}:\n" if $sym{$&};
	$o = substr($_, 12);
	$o =~ s/[\da-f]{4}/$sym{$&} ? $sym{$&} : $&/eg;
	print substr($_, 0, 12), $o;
}
close S;
exit 0;
