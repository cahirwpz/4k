AS	= vasm -quiet 
ASFLAGS = -sdreg=4 -x -Fhunk -m68040 -phxass -opt-pea -linedebug -showcrit

LD	= vlink
LDFLAGS	= -sc -sd -S -s -bamigahunk

OBJS	= startup.o main.o utils.o c2p.o cinter.o
BINARY	= intro4k.exe

all:	$(BINARY)

$(BINARY): $(OBJS)
	@echo "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^

DEPFILES := $(patsubst %.o,.deps/%.P,$(OBJS))

ifeq ($(words $(findstring $(MAKECMDGOALS), clean)), 0)
  -include $(DEPFILES)
endif

.deps/%.P: %.s
	@echo "DEP  $<"
	@mkdir -p .deps
	$(AS) $(ASFLAGS) -depend=make -o $(<:%.s=%.o) $< > $@

%.o: %.s .deps/%.P
	@echo "AS   $<"
	$(AS) $(ASFLAGS) -o $@ $<

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
