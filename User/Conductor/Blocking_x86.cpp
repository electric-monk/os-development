#include "Blocking.h"
#include "Handle_Internal.h"
#include "../../Kernel/Kernel/Blocking_Service.h"

namespace Kernel {

    namespace Blocking {
        
        Collections::Array* Blockable::Block(void)
        {
            int result;
            int list;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(list)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_BLOCK), "b"((int)this)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_BLOCK, result);
            return (Collections::Array*)list;
        }
        
        bool Blockable::IsBlocked(void)
        {
            int result;
            int value;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(value)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_IS_BLOCKED), "b"((int)this)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_IS_BLOCKED, result);
            return value != 0;
        }
        
        Collections::Array* Blockable::CurrentSignals(void)
        {
            int result;
            int list;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(list)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_LIST), "b"((int)this)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_LIST, result);
            return (Collections::Array*)list;
        }
        
        List* List::CreateOr(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_CREATE_OR)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_CREATE_OR, result);
            return (List*)handle;
        }
        
        List* List::CreateAnd(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_CREATE_AND)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_CREATE_AND, result);
            return (List*)handle;
        }
        
        void List::Add(Blockable *signal)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_ADD), "b"((int)this), "c"((int)signal)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_ADD, result);
        }
        
        void List::Remove(Blockable *signal)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_REMOVE), "b"((int)this), "c"((int)signal)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_REMOVE, result);
        }
        
        Timer* Timer::Create(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_CREATE_TIMER)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_CREATE_TIMER, result);
            return (Timer*)handle;
        }
        
        void Timer::Reset(int milliseconds, bool repeating)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_TIMER_RESET), "b"((int)this), "c"(milliseconds), "d"(repeating ? 1 : 0)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_TIMER_RESET, result);
        }
        
        void Timer::Cancel(void)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_TIMER_CANCEL), "b"((int)this)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_TIMER_CANCEL, result);
        }
        
        Simple* Simple::Create(bool activeLow)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_CREATE_SIMPLE), "b"(activeLow ? 1 : 0)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_CREATE_SIMPLE, result);
            return (Simple*)handle;
        }
        
        void Simple::Set(bool state)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(BLOCKING_SYSCALL), "a"(BLOCKING_FUNCTION_SIMPLE_SET), "b"((int)this), "c"(state ? 1 : 0)
                          : /* clobbers */);
            CheckResult(BLOCKING_SYSCALL, BLOCKING_FUNCTION_SIMPLE_SET, result);
        }

    }
    
}
