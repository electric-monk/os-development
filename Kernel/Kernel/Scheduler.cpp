#include "Scheduler.h"
#include "Thread.h"
#include "CPU.h"
#include "CPU_intr.h"

void Scheduler::BeginScheduling(void)
{
    while (true) {
        // Select a thread
        Thread *newThread = Thread::ThreadCursor();
        Thread::ThreadNext();
        
        // Jump into thread
        if (newThread != NULL) {
            Thread::Active = newThread;
            newThread->_context->SwitchFrom(&CPU::Active->scheduler);
            Thread::Active = NULL;
        } else {
            CPU_Interrupt_Enable();
            asm("hlt");
        }
    }
}

void Scheduler::EnterFromInterrupt(void)
{
    // Here, xv6 switched ot the kernel page map, e.g. its own kernel process. For efficiency, let's leave that for now
    if (Thread::Active != NULL)
        CPU::Active->scheduler->SwitchFrom(&Thread::Active->_context);
}
