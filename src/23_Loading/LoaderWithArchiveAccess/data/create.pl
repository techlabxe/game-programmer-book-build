#!/usr/bin/perl -w

for ( $i=0;$i<1000;++$i){
	open OUT,">$i.txt";
	$t = $i % 10;
	for ( $j=0;$j<$i;++$j){
		print OUT $t;
	}
	close OUT;
}
