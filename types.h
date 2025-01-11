#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef nil
#define nil	0
#endif

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define VRAM			((u8 *)0xf000)
#define VRAM2			((u8 *)0xf800)

// input
#define SRAND			(*(u32 *)0xfff4)
#define KEY				(*(u8 *)0xfffb)

#define KEY_UP			1
#define KEY_DOWN		2
#define KEY_LEFT		4
#define KEY_RIGHT		8
#define KEY_A			16
#define KEY_B			32

// output
#define SN				(*(u8 *)0xfff9)
#define DMAADR			(*(u8 *)0xfffa)
#define PUTCHAR			(*(u8 *)0xfffb)

#endif
