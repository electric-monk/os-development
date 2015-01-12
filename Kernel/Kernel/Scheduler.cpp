#include "Scheduler.h"
#include "Thread.h"
#include "CPU.h"
#include "CPU_intr.h"
#include "Process.h"
#include "debug.h"

void Scheduler::BeginScheduling(void)
{
    test('*');
    while (true) {
        // Select a thread
        Thread::ThreadNext();
        Thread *newThread = Thread::ThreadCursor();
        
        // Jump into thread
        if (newThread != NULL) {
            test('.');
            newThread->Select(&CPU::Active->scheduler);
        } else {
            test('&');
            rootAddressSpace.Select();
            Process::Active = NULL;
            CPU_Interrupt_Enable();
            asm("hlt");
            CPU_Interrupt_Disable();
        }
    }
}

void Scheduler::EnterFromInterrupt(void)
{
    test('!');
    // Here, xv6 switched ot the kernel page map, e.g. its own kernel process. For efficiency, let's leave that for now
    if (Thread::Active != NULL)
        CPU::Active->scheduler->SwitchFrom(&Thread::Active->_context);
}