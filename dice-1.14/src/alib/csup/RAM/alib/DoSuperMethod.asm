*** DO NOT EDIT: FILE BUILT AUTOMATICALLY
*** DoSuperMethod ram interface
	XREF	_None
	SECTION	alib
	XDEF	_DoSuperMethod
_DoSuperMethod:
		movem.l	a2/a6,-(a7)
		move.l	_None,a6
		lea	12(a7),a1
		movem.l	(a1)+,a0/a2
		jsr	-258(a6)
		movem.l	(a7)+,a2/a6
		rts
	END
