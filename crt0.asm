	.code
	.export	_idle
	.export	_interruptCount
	.export	_interruptHandler
	.export	_di
	.export	_ei
start:
	li	r0,intr
	mov	r0,@6		;level1 pc
	li	r13,0xf000	;for stack
	li	r0,__bss_size
	jeq	@nowipe
	li	r1,__bss
	clr	r2
wipe:
	mov	r2,*r1+
	dect	r0
	jne	@wipe
nowipe:
	limi	0xf
	bl	@_main
	ckof	; exit emulator
intr:
	inc	@_interruptCount
	mov	@_interruptHandler,r0
	jeq	@l1
	bl	*r0
l1:
	rtwp
_di:
	limi	0
	rt
_ei:
	limi	0xf
	rt
_idle:
	idle
	rt

_interruptCount:
	.word	0
_interruptHandler:
	.word	0
