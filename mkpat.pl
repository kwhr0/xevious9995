#! /usr/bin/perl

use Getopt::Std;
getopts("t", \%opts);
$transpose = $opts{"t"};

open(S, $ARGV[0]) || die;
binmode S;
read(S, $_, -s S);
close S;
@img = unpack("C*", $_);
$isbmp = $img[0] == 0x42 && $img[1] == 0x4d;
$imgofs = $isbmp ? $img[10] | $img[11] << 8 : 0x220;
$width = $isbmp ? ($img[18] | $img[19] << 8) + 3 >> 2 << 2 : 256;
$height = $isbmp ? $img[22] | $img[23] << 8 : 256;

open(S, "cut") || die;
open(O, "> pattern.s") || die;
print O "\t.export\t_bitmap\n\t.data\n_bitmap:\n";
while (<S>) {
	next if /^#/;
	chomp;
	($frameN, $x0c, $x0d, $y0c, $y0d, $x1c, $x1d, $y1c, $y1d, $name) = split(/\s/, $_);
	$frameN = hex($frameN);
	$x0c = hex($x0c);
	$y0c = hex($y0c);
	$x1c = hex($x1c);
	$y1c = hex($y1c);
	$xofs = 8 * $x0c + $x0d;
	$yofs = 8 * $y0c + $y0d;
	$pw = 8 * $x1c + $x1d - $xofs + 1;
	$ph = 8 * $y1c + $y1d - $yofs + 1;
	($pw, $ph) = ($ph, $pw) if $transpose;
	$xn = int(($pw + 15) / 16);
	$yn = int(($ph + 0) / 1);
	$info{"pat_" . $name} = sprintf "%d,%d,%d,%d", $frameN, $xn, $pw, $ph, $xn;
	for ($i = 0; $i < $frameN; $i++) {
		$key = sprintf "pat_%s%02d", $name, $i;
		$h{$key} = $ofs;
		for ($y = 0; $y < $yn; $y++) {
			for ($x = 0; $x < 16 * $xn; $x += 16) {
				$d = 0;
				$m = 0;
				for ($px = 0; $px < 16; $px++) {
					for ($py = 0; $py < 1; $py++) {
						$d <<= 1;
						$m <<= 1;
						$ty = $y + $py;
						next unless $ty >= 0 && $ty < $ph;
						$tx = $x + $px;
						$tx = $pw - 1 - $tx if $transpose;
						next unless $tx >= 0 && $tx < $pw;
						($tx, $ty) = ($ty, $tx) if $transpose;
						$c = $img[$imgofs + $xofs + $tx + $width * (
						$isbmp ? $height - 1 - ($yofs + $ty) :
						$yofs + $ty)] & 0xf;
						$d |= 1 if $c == 0xf;
						$m |= 1 if $c & 7;
					}
				}
				printf O "\t.word\t0x%04x,0x%04x\n", $d, $m;
				$ofs += 2;
			}
		}
		$xofs += 8 * ($x1c - $x0c + 1);
		if ($xofs >= $width) {
			$xofs = 0;
			$yofs += 8 * ($y1c - $y0c + 1);
		}
	}
}
close S;

$name = "";
open(H, "> pattern.h") || die;
foreach (sort keys %h) {
	next unless /(\D+)\d/;
	if ($1 ne $name) {
		print H "extern const Pattern ${t}[];\n" if $name;
		$t = $1;
		print O "\t.export\t_$t\n_$t:\n\t.byte\t$info{$t}\n";
	}
	$name = $1;
	printf O "\t.word\t0x%x\n", $h{$_};
}
print H "extern const Pattern ${t}[];\n";
close H;
close O;
exit 0;
