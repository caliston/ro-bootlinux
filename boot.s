

OS_EnterOS	*	&16
OS_Memory	*	&68

cp15	CP	15
c0	CN	0
c1	CN	1

	AREA	ASM, CODE, READONLY
	
; turn everything off, boot into Linux
; on entry:
;	a1 = size of kernel
;	a2 = Linux machine ID
;	a3 = pointer to ATAGS
;	a4 = physical address of Linux kernel
; on exit:
;	never returns!


	EXPORT	jump_to_linux
jump_to_linux
	;SWI	OS_EnterOS

; when we jump into physical memory, we need to make sure that we fix up the
; PC so it's also in physical memory else things will go horribly wrong
	STMFD	r13!,{a1-a4,r14}
	ADR	r3,code_in_phys_mem

logical_to_physical_translate
;	MOV	v3,r0
;	MOV	v4,r1
;	MOV	v5,r2
	SUB	r1,sp,#120+4	; reserve 12 bytes on the stack for page
				; block
	STR	r3,[r1,#4]	; translate logical address
	MOV	r0,#&2A00	; OS_Memory 0 (with flags)
	MOV	r2,#1
	SWI	OS_Memory
	LDR	v2,[r1,#8]	; into physical address
	;MOV	r0,v3
;	MOV	r1,v4
;	MOV	r2,v5
; done logical_to_physical_translate
	LDMFD	r13!,{a1-a4,r14}
	;mov	r0,v2
;	mov	pc,r14

	MRS	v1,CPSR
	BIC	v1,v1,#&DF
	ORR	v1,v1,#&D3	; IRQ off, FIQ off, no Thumb, SVC32 mode
	MSR	CPSR_all, v1

				; bit 22 = 0 (don't support rotated loads - reset value)
				; bit 13 = 0 (exception vectors low)
				; bit 12 = 0 (L1 I-cache off)
				; bit 11 = 0 (branch prediction off)
				; bit 9 = 0 (ROM protection off)
				; bit 8 = 0 (MMU protection off)
				; bit 7 = 0 (little endian)
				; bit 6-4: ignored
				; bit 3 = 0 (ARM7500 write buffer off)
				; bit 2 = 0 (D-cache off)
				; bit 1 = 0 (alignment exceptions off)
				; bit 0 = 0 (MMU off)
	MOV	v1,#&0
	MCR	cp15, 0, v1, c1,c0	; MMU and caches off
	; only two instructions remain in pipeline here
	MOV	pc,v2		; branch to routine now in physical memory

	MOV	r0,r0		; pointless NOPs, never executed
	MOV	r0,r0
	MOV	r0,r0

code_in_phys_mem
; now we're executing in physical memory space

; copy the kernel down to physical address zero
; (doesn't consider case where pages might clash)


	GBLL	CopyZero
CopyZero	SETL	{TRUE}

 [ CopyZero
copy_to_zero
	MOV	v1,#0
05
	LDR	v2,[a4,v1]	; copy from physical address we're supplied
	STR	v2,[v1]		; to zero (v1 is both address and counter)
	ADD	v1,v1,#4
	CMP	v1,a1		; done yet?
	BLE	%BT05


	MOV	pc,#0		; now jump into Linux for real
				; (a1 = physical address)
 |
	MOV	pc,a4
 ]


	END
