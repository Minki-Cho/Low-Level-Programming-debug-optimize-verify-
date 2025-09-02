.data
; declare externs for our Profiler functions
EXTERNDEF ProfileEnter:PROC
EXTERNDEF ProfileExit:PROC

.code

; Setup _penter
_penter PROC EXPORT
	push RAX
	push RCX
	push RDX
	push r8
	push r9
	push r10
	push r11
	push RBX ; Pushing extra register for 16-byte alignment

	mov RCX, [RSP + 40h] ; Return address

	sub RSP, 20h
	call OFFSET ProfileEnter
	add RSP, 20h

	pop RBX
	pop r11
	pop r10
	pop r9
	pop r8
	pop RDX
	pop RCX
	pop RAX

	ret
_penter ENDP

; Setup _pexit
_pexit PROC EXPORT
	push RAX
	push RCX
	push RDX
	push r8
	push r9
	push r10
	push r11
	push RBX

	mov RCX, [RSP + 40h]

	sub RSP, 20h
	call OFFSET ProfileExit
	add RSP, 20h

	pop RBX
	pop r11
	pop r10
	pop r9
	pop r8
	pop RDX
	pop RCX
	pop RAX

	ret
_pexit ENDP

END
