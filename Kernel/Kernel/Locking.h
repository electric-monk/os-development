#ifndef __LOCKING_H__
#define __LOCKING_H__

#define DEBUG_STACK_SIZE        10

class CPU;
typedef unsigned long LockInt32;

// Generic lock interface, provides some debugging and utilities but doesn't do any locking itself
class GenericLock
{
public:
    // These base implementations just record debug info
    virtual void Lock(void);
    virtual void Unlock(void);
    virtual bool Holding(void) = 0; // Does the current CPU hold this?
    
    class Autolock
    {
    public:
        Autolock(GenericLock *lock)
        {
            _lock = lock;
            _lock->Lock();
        }
        ~Autolock()
        {
            _lock->Unlock();
        }
    private:
        GenericLock *_lock;
    };
protected:
    GenericLock(const char *lockName);
    virtual ~GenericLock();
    
    // Debug info
    char *_name;
    CPU *_cpu;
    LockInt32 _lockStack[DEBUG_STACK_SIZE];
};

// This lock disables interrupts and spins for the item, so use sparingly
class HardcoreSpinLock : public GenericLock
{
public:
    HardcoreSpinLock(const char *name);
    
    void Lock(void);
    void Unlock(void);
    bool Holding(void);
    
private:
    LockInt32 _locked;
    LockInt32 _depth;
};

class InterruptableSpinLock
{
private:
    LockInt32 _locked;
    
public:
    InterruptableSpinLock();
    void Lock(void);
    void Unlock(void);
    
    class Autolock
    {
    private:
        InterruptableSpinLock *_lock;
    public:
        Autolock(InterruptableSpinLock *lock)
        {
            _lock = lock;
            _lock->Lock();
        }
        ~Autolock()
        {
            _lock->Unlock();
        }
    };
};

#endif // __LOCKING_H__
