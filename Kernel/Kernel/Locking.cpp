#include "Locking.h"
#include "tools.h"
#include "CPU.h"
#include "CPU_intr.h"

extern UInt32 virt;

// Weird method from xv6 source
static void GetCallerChain(void *v, UInt32 *output, UInt32 count)
{
    UInt32 *ebp;
    UInt32 i;
    
    ebp = (UInt32*)v - 2;
    for (i = 0; i < count; i++) {
        if ((ebp == NULL) || (ebp < &virt) || (ebp == (void*)0xFFFFFFFF))
            break;
        output[i] = ebp[1];
        ebp = (UInt32*)ebp[0];
    }
    for (; i < count; i++)
        output[i] = 0;
}

GenericLock::GenericLock(const char *lockName)
{
    if (lockName) {
        UInt32 nameLength = StringLength(lockName);
        _name = new char[nameLength + 1];
        CopyMemory(_name, lockName, nameLength + 1);
    } else _name = NULL;
    _cpu = NULL;
}

GenericLock::~GenericLock()
{
    if (_name)
        delete[] _name;
}

void GenericLock::Lock(void)
{
    _cpu = CPU::Active;
    GetCallerChain(this, _lockStack, DEBUG_STACK_SIZE);
}

void GenericLock::Unlock(void)
{
    _lockStack[0] = 0;
    _cpu = NULL;
}

HardcoreSpinLock::HardcoreSpinLock(const char *name)
:GenericLock(name)
{
    _locked = 0;
    _depth = 0;
}

void HardcoreSpinLock::Lock(void)
{
    CPU::Active->PushInterruptFlag();
    if (Holding()) {
        _depth++;
        return;
    }
    while (xchg(&_locked, 1) != 0);
    GenericLock::Lock();
}

void HardcoreSpinLock::Unlock(void)
{
    if (!Holding())
        /* panic */;
    if (_depth != 0) {
        _depth--;
        CPU::Active->PopInterruptFlag();
        return;
    }
    GenericLock::Unlock();
    xchg(&_locked, 0);
    CPU::Active->PopInterruptFlag();
}

bool HardcoreSpinLock::Holding(void)
{
    return _locked && (_cpu == CPU::Active);
}
