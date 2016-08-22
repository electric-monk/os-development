#include "Scheduler.h"
#include "Thread.h"
#include "CPU.h"
#include "CPU_intr.h"
#include "Process.h"
#include "debug.h"
#include "StandardPC.h"

void Scheduler::BeginScheduling(void)
{
    CPU_Interrupt_Disable();
    StartScheduling();
    while (true) {
        // Select a thread
        Thread *newThread;
        Thread *oldThread = Thread::ThreadCursor();
        do {
            Thread::ThreadNext();
            newThread = Thread::ThreadCursor();
        } while ((newThread->_state != tsRunnable) && (newThread != oldThread));
        if ((newThread == oldThread) && (newThread->_state != tsRunnable))
            newThread = NULL;
        
        // Jump into thread
        if (newThread != NULL) {
            newThread->_state = tsRunning;
            newThread->Select(&CPU::Active->scheduler);
            if (newThread->_state == tsRunning)
                newThread->_state = tsRunnable;
        } else {
            rootAddressSpace.Select();
            Process::Active = NULL;
            {
                InterruptEnabler enabler;
                asm volatile("hlt");
            }
        }
    }
}

void Scheduler::EnterFromInterrupt(void)
{
    // Here, xv6 switched ot the kernel page map, e.g. its own kernel process. For efficiency, let's leave that for now
    if (Thread::Active != NULL)
        CPU::Active->scheduler->SwitchFrom(&Thread::Active->_context);
}
