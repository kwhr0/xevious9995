#! /bin/sh
perl -ne 'printf "%s (%d%%)\n", $1, 100 * hex($1) / 0xf000 if /([\dA-F]{4}) B __end/;' a.map
