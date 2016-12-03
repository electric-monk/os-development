#ifndef __HANDLE_INTERNAL_H__
#define __HANDLE_INTERNAL_H__

#define CheckResult(x, y, z)    if(z != 0) _CheckResult(x, y, z)

namespace Kernel {

    void _CheckResult(unsigned char interruptNumber, unsigned char functionNumber, int result);

}

#endif // __HANDLE_INTERNAL_H__
