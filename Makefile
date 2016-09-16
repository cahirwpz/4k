AS	= vasm -quiet 
ASFLAGS = -sdreg=4 -x -Fhunk -m68040 -phxass -opt-pea -linedebug -showcrit

LD	= vlink
LDFLAGS	= -sc -sd -S -s -bamigahunk

BINARY	= intro4k.exe

all:	$(BINARY).packed

%.o: %.s
	@echo "AS   $<"
	$(AS) $(ASFLAGS) -o $@ $<

$(BINARY): startup.o main.o utils.o c2p.o cinter.o
	@echo "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^

%.packed: %
	@echo "PACK $@"
	Shrinkler $< $@ >/dev/null

clean:
	@rm -v -f *.o $(BINARY) $(BINARY).packed
	@rm -v -f *~

.PHONY: all clean

ifneq ($(VERBOSE), 1)
.SILENT:
endif
