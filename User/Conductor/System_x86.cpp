#include "System.h"
#include "Handle_Internal.h"
#include "../../Kernel/Kernel/System_Service.h"

namespace Kernel {
    
    namespace System {
        
        Driver* Driver::GetRootDriver(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(SYSTEM_SYSCALL), "a"(SYSTEM_DRIVER_GET_ROOT)
                          : /* clobbers */);
            CheckResult(SYSTEM_SYSCALL, SYSTEM_DRIVER_GET_ROOT, result);
            return (Driver*)handle;
        }
        
        Collections::Array* Driver::Children(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(SYSTEM_SYSCALL), "a"(SYSTEM_DRIVER_CHILDREN), "b"((int)this)
                          : /* clobbers */);
            CheckResult(SYSTEM_SYSCALL, SYSTEM_DRIVER_CHILDREN, result);
            return (Collections::Array*)handle;
        }
        
    }
    
}
