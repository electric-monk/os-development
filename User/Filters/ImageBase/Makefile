CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

#CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
CFLAGS = -I../../../Interfaces -I../../Library -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -q -melf_i386

OBJS = main.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

libimage.a: $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm *.o *.so libimage.a

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
