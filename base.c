#include "base.h"

u8 *vram = VRAM;

static u8 frameCount;

void memsetw(u16 *dst, u16 v, u16 n) {
	while (n--) *dst++ = v;
}

void memmovew(u16 *dst, u16 *src, u16 n) {
	if (src < dst) {
		dst += n;
		src += n;
		while(n--) *--dst = *--src;
	}
	else while(n--) *dst++ = *src++;
}

// format of "m"
// bit7    signed
// bit5    lower case
// bit4    zero prefixed
// bit3..0 radix-2
void putv(u16 v, s8 n, u8 m) {
	u8 c[17];
	s8 i = 0;
	u8 f, r = 2 + (m & 0xf), a = 7 + (m & 0x20), p = 0x20 + (m & 0x10);
	if (f = m & 0x80 && v & 0x8000) v = -v;
	do {
		u8 t = v % r + '0';
		if (t > '9') t += a;
		c[i++] = t;
	} while (v /= r);
	if (f) c[i++] = '-';
	if (n > 0) while (n-- > i) putchar(p);
	else n += i;
	do putchar(c[--i]);
	while (i);
	if (n < 0) while (n++) putchar(' ');
}

void puts_n(const char *s) {
	while (*s) putchar(*s++);
}

void puts(const char *s) {
	puts_n(s);
	putchar('\n');
}

static u32 rndv = 1;

void srand(void) {
	rndv = SRAND;
}

u16 rand(void) { // xorshift
	rndv = rndv ^ (rndv << 13);
	rndv = rndv ^ (rndv >> 17);
	rndv = rndv ^ (rndv << 5);
	return rndv & 0x7fff;
}

u16 R(u16 n) {
#if 1	// avoid compiler bug
	asm("dect r13");
	asm("mov r4,*r13");
	rand();
	asm("sla r1,1");
	asm("mpy *r13+,r1");
#else
	return (u32)n * rand() >> 15;
#endif
}

u8 waitVSync(u8 frames) {
	u8 dif = (u8)interruptCount - frameCount;
	u8 f, t = frameCount + frames;
	if (t >= frameCount)
		while ((f = interruptCount) >= frameCount && f < t)
			idle();
	else
		while ((f = interruptCount) >= frameCount || f < t)
			idle();
	frameCount = f;
	return dif;
}

u8 vramSwap(u8 frames) {
	u8 dif = waitVSync(frames);
	DMAADR = (u16)vram >> 8;
	vram = vram == VRAM ? VRAM2 : VRAM;
	return dif;
}

void vramSingle(void) {
	DMAADR = (u16)VRAM >> 8;
	vram = VRAM;
}
