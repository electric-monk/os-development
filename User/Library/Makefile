CC = i686-elf-gcc
AS = nasm
AR = i686-elf-ar

#CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
CFLAGS = -fno-builtin  -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -q -melf_i386

OBJS = entry.so runtime.o Handle_x86.o Collections_x86.o Blocking_x86.o Thread_x86.o System_x86.o IPC_x86.o BasicHeap.o Locking.o EventLoop.o Context.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

libsystem.a: $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm *.o *.so libsystem.a

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
