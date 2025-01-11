#include "base.h"
#include "bg.h"
#include "bgconf.h"

#define STAGE		1
#define LOOP		6
#define PRE_BLANK1	90
#define PRE_BLANK2	64
#define ADJUST		71

typedef struct {
	u16 *p;
	u16 d, r;
	u8 c, n;
} ExpandWork;

static ExpandWork w[8 * XN];
static u8 *yofsp;
static u8 stage;
static s16 xofs, xn;
static const u8 yofs[] = {
	58, 90, 13, 77, 32, 66, 0, 
	87, 52, 7, 77, 87, 40, 13, 67, 90
};
static u16 bg[XN / 2 * 136];
static u16 *bgp;

void bgInit(void) {
	stage = STAGE - 1;
	xofs = 0;
}

s16 bgOfs(void) {
	return stage * (BG_XN + PRE_BLANK2) + xofs - ADJUST;
}

u8 bgStage(void) {
	return stage + 1;
}

static void setup(void) {
	extern const u16 bgdata[];
	int ofs = (*yofsp << 2) + 2;
	ExpandWork *p = w;
	u16 *op = &bgdata[ofs];
	for (u8 x = 0; x < 8 * XN; x++) {
		p->p = &bgdata[*op++];
		p->d = 0; // compressed datum
		p->n = 0; // bit count of compressed datum
		p->c = 0; // current pixel ON/OFF
		p->r = 0; // run length
		p++;
	}
	if (++yofsp >= yofs + sizeof(yofs)) yofsp = (u8 *)yofs + LOOP;
}

static int get1bit(ExpandWork *p) {
	if (!p->n) {
		p->d = *p->p++;
		p->n = 16;
	}
	int r = (p->d & 0x8000) != 0;
	p->d <<= 1;
	p->n--;
	return r;
}

static void bgDraw() {
	ExpandWork *p = w;
	u16 *dp = bgp;
	for (int x = 0; x < XN >> 1; x++) {
		u16 d = 0;
		for (int i = 0; i < 16; i++) {
			if (!p->r) {
				if (get1bit(p)) { // 2-511pixels
					int i, r = 0;
					for (i = 0; i < 9; i++) r = r << 1 | get1bit(p);
					p->r = r;
				}
				else p->r = 1; // 1pixel
				p->c = !p->c;
			}
			p->r--;
			d <<= 1;
			if (!p->c) d |= 1;
			p++;
		}
		*dp++ = d;
	}
}

u8 bgUpdate(void) {
	if (xofs >= 0) bgDraw();
	int size = (u8 *)bg + sizeof(bg) - (u8 *)bgp;
	u8 *dst = vram + XN * 18;
	memmovew((u16 *)dst, bgp, size >> 1);
	memmovew((u16 *)(dst + size), bg, sizeof(bg) - size >> 1);
	bgp -= XN >> 1;
	if (bgp < bg) bgp = &bg[sizeof(bg) - XN >> 1];
	if (++xofs >= xn) {
		xofs = 0;
		xn = BG_XN + PRE_BLANK2;
		setup();
		if (++stage >= sizeof(yofs)) {
			stage = LOOP;
			return 1;
		}
	}
	return 0;
}

void bgStart(void) {
	memsetw(bg, 0, sizeof(bg) >> 1);
	bgp = &bg[sizeof(bg) - XN >> 1];
	if (xofs >= 7 * (BG_XN + PRE_BLANK2) / 10 && ++stage >= sizeof(yofs)) stage = LOOP;
	xofs = -PRE_BLANK1;
	xn = BG_XN + PRE_BLANK2;
	yofsp = (u8 *)yofs + stage;
	setup();
}
