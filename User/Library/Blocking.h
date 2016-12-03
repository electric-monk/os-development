#ifndef __BLOCKING_H__
#define __BLOCKING_H__

#include "Handle.h"

#define MICROSECONDS(x)         (x)
#define MILLISECONDS(x)         (MICROSECONDS(x) * 1000)
#define SECONDS(x)              (MILLISECONDS(x) * 1000)

namespace Kernel {
    
    namespace Collections {
        class Array;
    }
    
    namespace Blocking {

        class Blockable : public Handle
        {
        public:
            Collections::Array* Block(void);
            bool IsBlocked(void);
            Collections::Array* CurrentSignals(void);
        };
        
        class List : public Blockable
        {
        public:
            static List* CreateOr(void);
            static List* CreateAnd(void);
            
            void Add(Blockable *signal);
            void Remove(Blockable *signal);
        };
        
        class Timer : public Blockable
        {
        public:
            static Timer* Create(void);
            
            void Reset(int microseconds, bool repeating);
            void Cancel(void);
        };
        
        class Simple : public Blockable
        {
        public:
            static Simple* Create(bool activeLow);
            
            void Set(bool state);
        };
        
    }
    
}

#endif // __BLOCKING_H__
