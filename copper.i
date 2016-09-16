; CMOVE register, word
CMOVE   macro
        dc.w    \1 & $1ff
        dc.w    \2
        endm

; CLONG register, long
CLONG   macro
        dc.w    (\1 + 2) & $1ff
        dc.w    \2 >> 16
        dc.w    \1 & $1ff
        dc.w    \2 & $ffff
        endm

; CWAIT vpos, hpos
CWAIT   macro
        dc.b    \1
        dc.b    \2 ! 1
        dc.w    $fffe
        endm

CEND    macro
        dc.l    $fffffffe
        endm

CDDFSET macro
\@      set     (((\1 - 17) & ~7) / 2)
        CMOVE   ddfstrt, \@
        CMOVE   ddfstop, (\@ + \2 / 2 - 8)
        endm

CDIWSET macro
        CMOVE   diwstrt, ((\2 & $ff) << 8) | (\1 & $ff)
        CMOVE   diwstop, (((\2 + \4) & $ff) << 8) | ((\1 + \3) & $ff)
        endm

; vim: ts=8 sw=8 ft=asm68k
