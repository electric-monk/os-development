CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld

#CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
CFLAGS = -fno-builtin -I../../Interfaces -I../Library -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -static -L../Library -q -melf_i386

OBJS = main.o Window.o Controls.o TestFont.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

Conductor.task: $(OBJS)
	$(LD) $(LFLAGS) -T User.ld -Map conductor.map -o $@ $^ -lsystem

clean:
	rm *.o *.so Conductor.task

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
