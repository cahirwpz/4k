        include 'custom.i'

        xdef    wait_vbl
        xdef    wait_lmb
        xdef    get_frame
        xdef    load_palette

        section code_f, code, fast

        near    a4

wait_lmb:
.loop   btst    #6,$bfe001
        bne.b   .loop
        rts

wait_vbl:
.loop1  move.l  vposr(a5),d0
        and.l   #$1ff00,d0
        cmp.l   #303<<8,d0
        bne.b   .loop1

.loop2  move.l  vposr(a5),d0
        and.l   #$1ff00,d0
        cmp.l   #303<<8,d0
        beq.b   .loop2
        rts

get_frame:
        lea     ciaa,a0
        clr.l   d0
        move.b  ciatodhi(a0),d0
        lsl.l   #8,d0
        move.b  ciatodmid(a0),d0
        lsl.l   #8,d0
        move.b  ciatodlow(a0),d0
        rts

load_palette:
        lea     color(a5),a1
        clr.l   d2

.loop   move.l  (a0)+,d0        ; high color nibble
        move.l  d0,d1           ; low color nibble
        lsr.l   #4,d0        
        lsl.b   #4,d0
        lsl.b   #4,d1
        lsl.w   #4,d0
        lsl.w   #4,d1
        lsr.l   #8,d0
        lsr.l   #8,d1
        and.w   #$fff,d0
        and.w   #$fff,d1

        ; [d3] color register index
        move.w  d2,d3
        and.w   #$1f,d3

        ; [d4] color register bank
        move.w  d2,d4
        lsl.w   #8,d4
        and.w   #BPLCON3_BANK0!BPLCON3_BANK1!BPLCON3_BANK2,d4

        move.w  d4,bplcon3(a5)  ; switch to high color nibble
        move.w  d0,(a1,d3.w*2)
        or.w    #BPLCON3_LOCT,d4
        move.w  d4,bplcon3(a5)  ; switch to low color nibble
        move.w  d1,(a1,d3.w*2)

        addq.b  #1,d2
        bcc.b   .loop
        rts

; vim: ts=8 sw=8 ft=asm68k
