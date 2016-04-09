CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
#CFLAGS = -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -q -melf_i386

OBJS = main.o entry.so runtime.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

Conductor.task: $(OBJS)
	$(LD) $(LFLAGS) -T User.ld -o $@ $^

clean:
	rm *.o *.so Conductor.task

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
