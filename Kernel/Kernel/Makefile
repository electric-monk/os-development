CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector
AFLAGS = -f elf
LFLAGS = -melf_i386

OBJS = entry.so main.o debug.o tools.o runtime.o descript.o descript_cpu.so BasicHeap.o mem_logical.o mem_physical.o Driver.o StandardPC.o StandardPC_traps.so CPU.o CPU_PICIRQ.o CPU_context.so Thread.o Thread_init.so Scheduler.o Process.o process_test.so

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: $(OBJS)
	$(LD) $(LFLAGS) -T Kernel.ld -Map kernel.map -o $@ $^

clean:
	rm *.o *.so kernel.elf

depend: .depend
.depend: $(SRC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend