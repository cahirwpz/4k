        include 'hardware/custom.i'
        include 'hardware/dmabits.i'
        include 'hardware/intbits.i'
        include 'hardware/cia.i'

ciaa                    equ     $bfe001
ciab                    equ     $bfd000

bplpt0                  equ     $0e0
bplpt1                  equ     $0e4
bplpt2                  equ     $0e8
bplpt3                  equ     $0ec
bplpt4                  equ     $0f0
bplpt5                  equ     $0f4
bplpt6                  equ     $0f8
bplpt7                  equ     $0fc

BPLCON0_ECSENA          equ     $0001
BPLCON0_LACE            equ     $0002
BPLCON0_BPU3            equ     $0010
BPLCON0_SHIRES          equ     $0040
BPLCON0_GAUD            equ     $0100
BPLCON0_COLOR           equ     $0200
BPLCON0_DBLPF           equ     $0400
BPLCON0_HOMOD           equ     $0800
BPLCON0_BPU0            equ     $1000
BPLCON0_BPU1            equ     $2000
BPLCON0_BPU2            equ     $4000
BPLCON0_HIRES           equ     $8000

BPLCON1_PF2H3           equ     $0080
BPLCON1_PF2H2           equ     $0040
BPLCON1_PF2H1           equ     $0020
BPLCON1_PF2H0           equ     $0010
BPLCON1_PF1H3           equ     $0008
BPLCON1_PF1H2           equ     $0004
BPLCON1_PF1H1           equ     $0002
BPLCON1_PF1H0           equ     $0001

BPLCON2_PF2PRI          equ     $0040
BPLCON2_PF2P2           equ     $0020
BPLCON2_PF2P1           equ     $0010
BPLCON2_PF2P0           equ     $0008
BPLCON2_PF1P2           equ     $0004
BPLCON2_PF1P0           equ     $0002
BPLCON2_PF1P1           equ     $0001

BPLCON3_BRDRTRAN        equ     $0010
BPLCON3_BRDRBLNK        equ     $0020
BPLCON3_SPRES1          equ     $0040
BPLCON3_SPRES0          equ     $0080
BPLCON3_LOCT            equ     $0200
BPLCON3_BANK0           equ     $2000
BPLCON3_BANK1           equ     $4000
BPLCON3_BANK2           equ     $8000

BEAMCON0_DISPLAYPAL     equ     $0020

INT_LEVEL3              equ     $6C

; vim: ts=8 sw=8 ft=asm68k
