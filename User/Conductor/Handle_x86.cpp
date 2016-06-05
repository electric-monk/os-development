#include "Handle.h"
#include "Handle_Internal.h"
#include "../../Kernel/Kernel/KernelObject_Service.h"

namespace Kernel {
    
    void Handle::AddRef(void)
    {
        int result;
        asm volatile ("int %[interrupt]"
                    : /* outputs */ "=a"(result)
                    : /* inputs */ [interrupt]"N"(KERNELOBJECT_SYSCALL), "a"(KERNELOBJECT_FUNCTION_ADDREF), "b"((int)this)
                    : /* clobbers */);
        CheckResult(KERNELOBJECT_SYSCALL, KERNELOBJECT_FUNCTION_ADDREF, result);
    }
    
    void Handle::Release(void)
    {
        int result;
        asm volatile ("int %[interrupt]"
                      : /* outputs */ "=a"(result)
                      : /* inputs */ [interrupt]"N"(KERNELOBJECT_SYSCALL), "a"(KERNELOBJECT_FUNCTION_RELEASE), "b"((int)this)
                      : /* clobbers */);
        CheckResult(KERNELOBJECT_SYSCALL, KERNELOBJECT_FUNCTION_RELEASE, result);
    }
    
    int Handle::Hash(void)
    {
        int result;
        int hash;
        asm volatile ("int %[interrupt]"
                      : /* outputs */ "=a"(result), "=b"(hash)
                      : /* inputs */ [interrupt]"N"(KERNELOBJECT_SYSCALL), "a"(KERNELOBJECT_FUNCTION_HASH), "b"((int)this)
                      : /* clobbers */);
        CheckResult(KERNELOBJECT_SYSCALL, KERNELOBJECT_FUNCTION_HASH, result);
        return hash;
    }
    
    bool Handle::IsEqual(Handle *other)
    {
        int result;
        int isEqual;
        asm volatile ("int %[interrupt]"
                      : /* outputs */ "=a"(result), "=b"(isEqual)
                      : /* inputs */ [interrupt]"N"(KERNELOBJECT_SYSCALL), "a"(KERNELOBJECT_FUNCTION_EQUAL), "b"((int)this), "c"((int)other)
                      : /* clobbers */);
        CheckResult(KERNELOBJECT_SYSCALL, KERNELOBJECT_FUNCTION_EQUAL, result);
        return isEqual != 0;
    }
    
    int Handle::GetClassName(int superclassCount, char *outputBuffer, int maxLength)
    {
        int result;
        int outLength;
        asm volatile ("int %[interrupt]"
                      : /* outputs */ "=a"(result), "=b"(outLength)
                      : /* inputs */ [interrupt]"N"(KERNELOBJECT_SYSCALL), "a"(KERNELOBJECT_FUNCTION_GETNAME), "b"((int)this), "c"((int)superclassCount), "d"((int)outputBuffer), "D"(maxLength)
                      : /* clobbers */);
        CheckResult(KERNELOBJECT_SYSCALL, KERNELOBJECT_FUNCTION_GETNAME, result);
        return outLength;
    }
    
    void _CheckResult(unsigned char interruptNumber, unsigned char functionNumber, int result)
    {
        if (result != KERNELOBJECT_ERROR_SUCCESS) {
            // TODO: Throw an exception, once they're allowed
        }
    }
    
}
