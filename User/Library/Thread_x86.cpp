#include "Thread.h"
#include "Handle_Internal.h"
#include "../../Kernel/Kernel/Thread_Service.h"

namespace Kernel {
    
    namespace Thread {
        
        Thread* Thread::Create(ThreadEntry entrypoint)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(THREAD_SYSCALL), "a"(THREAD_FUNCTION_CREATE), "b"((int)entrypoint)
                          : /* clobbers */);
            CheckResult(THREAD_SYSCALL, THREAD_FUNCTION_CREATE, result);
            return (Thread*)handle;
        }
            
        void Thread::Kill(void)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(THREAD_SYSCALL), "a"(THREAD_FUNCTION_KILL), "b"((int)this)
                          : /* clobbers */);
            CheckResult(THREAD_SYSCALL, THREAD_FUNCTION_KILL, result);
        }
            
        void Thread::Terminate(void)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(THREAD_SYSCALL), "a"(THREAD_FUNCTION_TERMINATE)
                          : /* clobbers */);
            // Should never get here...
            CheckResult(THREAD_SYSCALL, THREAD_FUNCTION_TERMINATE, result);
        }
        
        Memory* Memory::Create(int maximumSize, void **addressOut)
        {
            int result;
            int handle;
            int address;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle), "=c"(address)
                          : /* inputs */ [interrupt]"N"(THREAD_SYSCALL), "a"(THREAD_FUNCTION_ALLOC), "b"(maximumSize)
                          : /* clobbers */);
            CheckResult(THREAD_SYSCALL, THREAD_FUNCTION_ALLOC, result);
            if (addressOut)
                *addressOut = (void*)address;
            return (Memory*)handle;
        }
            
        void* Memory::Address(void)
        {
            int result;
            int address;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(address)
                          : /* inputs */ [interrupt]"N"(THREAD_SYSCALL), "a"(THREAD_FUNCTION_GET_ADDRESS), "b"((int)this)
                          : /* clobbers */);
            CheckResult(THREAD_SYSCALL, THREAD_FUNCTION_GET_ADDRESS, result);
            return (void*)address;
        }

    }
    
}
