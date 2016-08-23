CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -g -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
#CFLAGS = -m32 -O3 -fno-rtti -fno-exceptions -fno-stack-protector -std=c++11
AFLAGS = -f elf
LFLAGS = -melf_i386

OBJS = entry.so main.o debug.o tools.o runtime.o descript.o descript_cpu.so BasicHeap.o mem_logical.o mem_physical.o Driver.o StandardPC.o StandardPC_traps.so CPU.o CPU_PICIRQ.o CPU_context.so Thread.o Thread_init.so Scheduler.o Process.o mem_virtual.o Collections.o KernelObject.o Blocking.o Locking.o Driver_ATA.o IPC.o Queue.o Provider.o fs_iso9660.o ImageLoader.o FileNubbin.o Runloop.o Startup.o GenericKeyboard.o pci.o Video_Multiboot.o Console.o GenericVideo.o Collections_Service.o Blocking_Service.o KernelObject_Service.o Thread_Service.o Userspace_Heap.o System_Service.o IPC_Manager.o ConvenientSink.o GenericMouse.o IPC_Service.o

SRC = $(patsubst %.so,%.asm,$(patsubst %.o,%.cpp,$(OBJS)))
SRCC = $(filter %.cpp,$(SRC))

%.so: %.asm
	$(AS) $(AFLAGS) -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: $(OBJS)
	$(LD) $(LFLAGS) -T Kernel.ld -Map kernel.map -o $@ $^

clean:
	rm *.o *.so kernel.elf

documentation:
	doxys DoxySfile

depend: .depend
.depend: $(SRCC)
	rm -rf ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend
include .depend
