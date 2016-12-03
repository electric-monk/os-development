#ifndef __THREAD_H__
#define __THREAD_H__

#include "Handle.h"

namespace Kernel {
    
    namespace Thread {
        
        typedef void (*ThreadEntry)(void);
        
        class Thread : public Handle
        {
        public:
            static Thread* Create(ThreadEntry entrypoint);
            
            void Kill(void);
            
            static void Terminate(void);
        };
        
        class Memory : public Handle
        {
        public:
            static Memory* Create(int maximumSize, void **addressOut);
            
            void* Address(void);
        };
        
    }
    
}

#endif // __THREAD_H__
