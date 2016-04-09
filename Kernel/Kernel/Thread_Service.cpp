#include "Thread.h"
#include "Thread_Service.h"
#include "Interrupts.h"
#include "Process.h"
#include "Userspace_Heap.h"

extern UInt32 virt;

namespace Thread_Internal {
    class Service : public SystemService
    {
    protected:
        void Handle(UInt64 *parameters)
        {
            switch (parameters[0]) {
                default:
                    parameters[0] = THREAD_ERROR_BAD_FUNCTION;
                    return;
                    
                case THREAD_FUNCTION_CREATE:
                    if (parameters[1] >= virt) {
                        parameters[0] = THREAD_ERROR_PROTECTED_ADDRESS;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(new Thread(Process::Active, (void(*)(void*))parameters[1], NULL));
                    break;
                    
                case THREAD_FUNCTION_KILL:
                {
                    Thread *thread = (Thread*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!thread || !thread->IsDerivedFromClass("Thread")) {
                        parameters[0] = THREAD_INVALID_HANDLE;
                        return;
                    }
                    thread->Kill();
                }
                    break;
                case THREAD_FUNCTION_ALLOC:
                {
                    VirtualMemory *memory = new UserspaceHeap(Process::Active, (UInt32)parameters[0]);
                    parameters[1] = Process::Mapper()->Map(memory);
                    parameters[2] = (UInt64)memory->LinearBase();
                }
                    break;
                case THREAD_FUNCTION_TERMINATE:
                    Thread::Active->Kill(); // We won't return from this
                    break;
            }
            parameters[0] = THREAD_ERROR_SUCCESS;
        }

        int Interrupt(void)
        {
            return THREAD_SYSCALL;
        }
    };
}

static Thread_Internal::Service service;

void Thread::ConfigureService(void)
{
    service.Register();
}
