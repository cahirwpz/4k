        include 'lvo/exec_lib.i'
        include 'lvo/graphics_lib.i'

        include 'exec/execbase.i'
        include 'exec/tasks.i'
        include 'exec/macros.i'
        include 'exec/nodes.i'
        include 'dos/dosextens.i'
        include 'graphics/gfxbase.i'

        include 'custom.i'
        include 'env.i'

        xdef    IntVecBase

        xref    wait_vbl
        xref    _main
        xref    _LinkerDB

; graphics.library registers IVBLIT interrupt vector
; and puts its base into IV_DATA field
GFXBase equ     IVBLIT+IV_DATA

        section code_f, code, fast

_start:
        ; Initialize base relative mode
        lea     _LinkerDB,a4
        near    a4

        move.l  $4.w,a6

        ; Find current task address
        sub.l   a1,a1
        JSRLIB  FindTask
        move.l  d0,a2

        ifne    DEBUG
        lea     UAEDebug(pc),a1
        move.l  a1,TC_TRAPCODE(a2)
        endc

.okdos  ; Allocate space for WBMessage
        clr.l   -(sp)
        clr.l   d4                      ; [d4] exit code

        ; Is it a CLI process ?
        move.l  pr_CLI(a2),d0
        bne.s   .chkrom

.wb     ; if not then wait for WBMessage
        lea     pr_MsgPort(a2),a0
        JSRLIB  WaitPort
        ; Receive WBMessage and store it
        lea     pr_MsgPort(a2),a0
        JSRLIB  GetMsg
        move.l  d0,(sp)

.chkrom ; Check for Kickstart 3.0 and later
        cmp.w   #39,SoftVer(a6)
        bge.b   .chkcpu
        
.badrom moveq.l #10,d4
        bra.s   .exit

.chkcpu ; Check for 68040 with FPU and 68881 emulation
        move.w  AttnFlags(a6),d0
        btst.b  #AFB_68040,d0
        beq.s   .badcpu
        btst.b  #AFB_FPU40,d0
        beq.s   .badcpu
        btst.b  #AFB_68881,d0
        bne.s   .main

.badcpu moveq.l #20,d4 
        bra.s   .exit

.main   ; Disable multitasking
        JSRLIB  Forbid

        ; Retreive Vector Base Regiser
        lea     GetVBR(pc),a5
        JSRLIB  Supervisor
        move.l  d0,IntVecBase(a4)

        ; Intercept the view of AmigaOS
        move.l  GFXBase(a6),a6
        move.l  gb_ActiView(a6),-(sp)
        suba.l  a1,a1
        JSRLIB  LoadView
        JSRLIB  WaitTOF
        JSRLIB  WaitTOF

        ; [a5] globally set to $dff000, do not change!
        lea     $dff000,a5

        ; Ensure PAL mode
        move.w  #BEAMCON0_DISPLAYPAL,beamcon0(a5)

        ; Save DMA and interrupts state
        move.w  dmaconr(a5),d6          ; [d6] dmacon
        or.w    #DMAF_SETCLR,d6
        move.w  intenar(a5),d7          ; [d7] intena
        or.w    #INTF_SETCLR,d7

        ; Disable DMA & interrupts
        move.w  #$7fff,d5               ; [d5] turn off everything!
        move.w  d5,dmacon(a5)
        move.w  d5,intena(a5)
        bsr     wait_vbl

        ; Clear all interrupt requests
        move.w  d5,intreq(a5)

        ; Enable master switches for DMA and interrupts
        move.w  #DMAF_SETCLR!DMAF_MASTER,dmacon(a5)
        move.w  #INTF_SETCLR!INTF_INTEN,intena(a5)

        movem.l d4-d7/a4-a6,-(sp)
        bsr     _main
        movem.l (sp)+,d4-d7/a4-a6

        ; Disable DMA & interrupts
        move.w  d5,dmacon(a5)
        move.w  d5,intena(a5)
        bsr     wait_vbl

        ; Clear all interrupt requests
        move.w  d5,intreq(a5)

        ; REstore AmigaOS state of dma & interrupts,
        move.w  d6,dmacon(a5)
        move.w  d7,intena(a5)

        ; ... old copper list,
        move.l  gb_copinit(a6),cop1lc(a5)
        bsr     wait_vbl

        ; ...and original view.
        move.l  (sp)+,a1
        JSRLIB  LoadView

        ; Enable multitasking
        move.l  $4.w,a6
        JSRLIB  Permit

.exit   ; Reply WBMessage if there's one
        move.l  (sp)+,d2
        beq.s   .endcli

        JSRLIB  Forbid
        move.l  d2,a1
        JSRLIB  ReplyMsg

.endcli ; Set exit code
        move.l  d4,d0
        rts

GetVBR:
        movec   vbr,d0
        rte

        ifne    DEBUG
UAEDebug:
        addq.l  #4,sp
        jmp     $f0ff68
        endc

        section bss_f, bss, fast

IntVecBase      ds.l    1

; vim: ts=8 sw=8 filetype=asm68k
