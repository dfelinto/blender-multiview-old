#!/usr/bin/env perl

$linelength = 72;
$indent = "  ";

while (<>)
{
	@words = split " ", $_;
	$loc = length($indent);
	printf "$indent";
	foreach $word (@words){
		if ((length($word) + 1 + $loc) > $linelength) {
			printf "\n$indent";
			$loc = length($indent);;
		}
		printf "$word ";
		$loc += length($word) + 1;
	}
	printf "\n";
}

