AS = as9900
OBJS = crt0.o main.o base.o mathi.o music.o play.o tone.o \
	game.o chr.o emitter.o es.o sp.o sprite.o pattern.o bgdata.o bg.o \
	title.o name.o

all: pattern.h $(OBJS)
	ld9900 -A 2 -b -C 256 -m a.map -o a.out \
	$(OBJS) /opt/cc9995/lib/libc.a /opt/cc9995/lib/lib9995.a
	/opt/cc9995/xdt99/xda99.py -a 0 -f 0 -5 a.out
	./mkadr.pl > a.adr
	./mklst.pl > a.lst
	./total.sh

crt0.o: crt0.asm
	$(AS) $<

bgdata.s: mkbg.pl map.bmp
	./mkbg.pl map.bmp

es.c: essrc mkes.pl
	./mkes.pl $< > $@

music.h music.s: mkmusic
	./mkmusic

mkmusic: msrc.cpp mkmusic.h mml.c tone.c
	c++ -std=c++11 -Wno-deprecated -o $@ $< mml.c tone.c

tone.c: mktone.pl
	./mktone.pl >$@

pattern.h pattern.s: mkpat.pl sprite.tim
	./mkpat.pl sprite.tim
	
.c.o:
	cc9995 -m9995 -S $<
	$(AS) -l $(<:.c=.lst) $(<:.c=.s)

clean:
	rm -f bgconf.h es.c mkmusic music.h pattern.* tone.c
	rm -f *.{lst,o,s} a.{adr,map,out,dis}
