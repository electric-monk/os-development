CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
SR = ../../../utils/ParseData/ParseData/a.out

#CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
CFLAGS = -I../../../Interfaces -I../../Library -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -static -L../../Library -L ../ImageBase -q -melf_i386

OBJS = png.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.bin: %.txt
	$(SR) tobin $< $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

png.filter: $(OBJS) config.bin
	$(LD) $(LFLAGS) -T User.ld -o $@ $^ -\( -limage -lsystem -\)

clean:
	rm *.o *.so png.filter

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
