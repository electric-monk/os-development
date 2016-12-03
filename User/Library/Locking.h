#ifndef __LOCKING_H__
#define __LOCKING_H__

#include "Runtime.h"

namespace Locking {

    class GenericLock
    {
    public:
        GenericLock();
        virtual ~GenericLock();
        
        virtual bool TryLock(void) = 0;
        virtual void Lock(void) = 0;
        virtual void Unlock(void) = 0;
    };
    
    class Spinlock : public GenericLock
    {
    public:
        Spinlock();
        
        bool TryLock(void);
        void Lock(void);
        void Unlock(void);
        
    private:
        UInt32 _lock;
    };
    
}

#endif // __LOCKING_H__
