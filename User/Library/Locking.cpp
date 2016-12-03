#include "Locking.h"
#include "Tools.h"

namespace Locking {
    
    GenericLock::GenericLock()
    {
    }
    
    GenericLock::~GenericLock()
    {
    }
    
    Spinlock::Spinlock()
    {
        _lock = 0;
    }
    
    bool Spinlock::TryLock(void)
    {
        return xchg(&_lock, 1) == 0;
    }
    
    void Spinlock::Lock(void)
    {
        while (xchg(&_lock, 1) != 0);
    }
    
    void Spinlock::Unlock(void)
    {
        xchg(&_lock, 0);
    }
    
}