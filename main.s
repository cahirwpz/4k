        include 'copper.i'
        include 'custom.i'
        include 'env.i'

        xdef    _main
        xref    wait_lmb
        xref    load_palette
        xref    c2p1x1_8_c5_gen_mini

        ifne    MUSIC
        xref    CinterInit
        xref    CinterPlay1
        xref    CinterPlay2
        xref    IntVecBase
        endc

        section code_f, code, fast

        near    a4

_main:

        ifne    MUSIC
        bsr     CinterInit

        move.l  IntVecBase(a4),a0
        move.l  INT_LEVEL3(a0),-(sp)
        lea     vbl_interrupt(pc),a1
        move.l  a1,INT_LEVEL3(a0)
        move.w  #INTF_SETCLR!INTF_INTEN!INTF_VERTB,intena(a5)
        endc

        bsr     make_palette
        lea     palette(a4),a0
        bsr     load_palette

        bsr     render_frame
        bsr     chunky_to_planar
        bsr     swap_screen
        
        move.l  #coplist,cop1lc(a5)
        clr.w   copjmp1(a5)
        move.w  #DMAF_SETCLR!DMAF_MASTER!DMAF_COPPER!DMAF_RASTER,dmacon(a5)

        bsr     wait_lmb

        ifne    MUSIC
        move.l  IntVecBase(a4),a0
        move.l  (sp)+,INT_LEVEL3(a0)
        endc

        rts

        ifne    MUSIC
vbl_interrupt:
        movem.l d0-d7/a0-a6,-(sp)

        bsr     CinterPlay1
        bsr     CinterPlay2

        move.w  #INTF_VERTB,$dff000+intreq

        movem.l (sp)+,d0-d7/a0-a6
        rte
        endc

render_frame:
        lea     chunky,a0
        clr.w   d1
.loop_y clr.w   d0
.loop_x cmp.b   d1,d0
        blo.s   .lo
        move.b  d1,(a0)+
        bra.s   .hi
.lo     move.b  d0,(a0)+
.hi     addq.w  #1,d0
        cmp.w   #WIDTH,d0
        blt.s   .loop_x
        addq.w  #1,d1
        cmp.w   #HEIGHT,d1
        blt.s   .loop_y
        rts

current_screen:
        move.l  screen(a4),d0
        mulu.l  #WIDTH*HEIGHT,d0
        add.l   #bitplanes,d0
        addq.l  #7,d0                   ; make sure bpls are 64-bit aligned
        and.w   #$fff8,d0
        rts

swap_screen:
        bsr     current_screen

        bchg.b  #0,screen+3(a4)         ; [vasm] bchg.l does not work :(

        lea     coplist_bplpt+2,a0

        moveq   #DEPTH-1,d1
.loop   move.w  d0,(a0)
        swap    d0
        move.w  d0,4(a0)
        swap    d0
        add.l   #(WIDTH*HEIGHT)/DEPTH,d0
        addq.l  #8,a0
        dbra    d1,.loop

        rts

chunky_to_planar:
        bsr     current_screen
        move.l  d0,a1
        lea     chunky,a0

        movem.l d2-d7/a2-a6,-(sp)
        bsr     c2p1x1_8_c5_gen_mini
        movem.l (sp)+,d2-d7/a2-a6
        rts

make_palette:
        lea     palette(a4),a0
        clr.l   d0
        clr.l   d1
.loop   move.l  d0,(a0)+
        add.l   #$010101,d0
        addq.b  #1,d1
        bcc.b   .loop
        rts

        section bss_f, bss, fast

screen:
        ds.l    1

palette:
        ds.l    256

        section data_c, data, chip

coplist:
        CMOVE   bplcon0, BPLCON0_BPU3!BPLCON0_COLOR!BPLCON0_ECSENA
        CMOVE   bplcon1, 0
        CMOVE   bplcon2, BPLCON2_PF2P2!BPLCON2_PF1P2!BPLCON2_PF2PRI
        CMOVE   bplcon3, BPLCON3_BRDRBLNK
        CDIWSET CX, CY, WIDTH, HEIGHT
coplist_bplpt:
        CLONG   bplpt0, 0
        CLONG   bplpt1, 0
        CLONG   bplpt2, 0
        CLONG   bplpt3, 0
        CLONG   bplpt4, 0
        CLONG   bplpt5, 0
        CLONG   bplpt6, 0
        CLONG   bplpt7, 0
        CMOVE   bpl1mod, -8     ; because fmode = 3
        CMOVE   bpl2mod, -8
        CDDFSET CX, WIDTH
        CMOVE   fmode, 3
        CMOVE   color, $f00
        CEND

        section bss_f, bss, fast

chunky:
        ds.b    WIDTH*HEIGHT

        section bss_c, bss, chip

bitplanes:
        ds.b    WIDTH*HEIGHT*2+4

; vim: ts=8 sw=8 ft=asm68k
