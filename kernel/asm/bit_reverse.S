@@
@ @file   bit_reverse.S
@
@ @brief  reorder output after FFT, ARM Cortex-M4 implementation
@ @note   Not for public release, do not share
@
@ @date   last modified 5 October 2023
@ @author CMU 14-642 
@@

.syntax unified
.section .text
.thumb

@@
@ @brief  in-place post-FFT bit reversal
@ @param  uint16_t *p - input/output buffer
@ @param  uint16_t len - bit reverse table size
@ @param  uint16_t *table - bit reverse table
@ @return none
@@

.global bit_reverse

bit_reverse:
	ADDS     r3,r1,#1
	CMP      r3,#1
	IT       LS
	BXLS     lr
	PUSH     {r4-r9}
	ADDS     r1,r2,#2
	LSRS     r3,r3,#2
bit_reverse_0:
	LDRH     r8,[r1,#4]
	LDRH     r9,[r1,#2]
	LDRH     r2,[r1,#0]
	LDRH     r12,[r1,#-2]
	ADD      r8,r0,r8,LSR #1
	ADD      r9,r0,r9,LSR #1
	ADD      r2,r0,r2,LSR #1
	ADD      r12,r0,r12,LSR #1
	LDR      r7,[r9,#0]
	LDR      r6,[r8,#0]
	LDR      r5,[r2,#0]
	LDR      r4,[r12,#0]
	STR      r6,[r9,#0]
	STR      r7,[r8,#0]
	STR      r5,[r12,#0]
	STR      r4,[r2,#0]
	ADDS     r1,r1,#8
	SUBS     r3,r3,#1
	BNE      bit_reverse_0
	POP      {r4-r9}
	BX       lr


@@
@ @brief  clear some RAM to keep bootloader fresh
@ @param  none
@ @return none
@@

.global prep_for_reset

prep_for_reset:
    ldr r1, =#0x20000000
    ldr r2, =#0x00001000
    ldr r0, =#0x55555555
prep_reset_loop:
    subs r2, #4
    str r0, [r1, r2]
    bgt prep_reset_loop
    bx lr
