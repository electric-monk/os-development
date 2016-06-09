#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "Handle.h"
#include "Collections.h"

namespace Kernel {
    
    namespace System {
    
        class Driver : public Handle
        {
        public:
            static Driver* GetRootDriver(void);
            Collections::Array* Children(void);
        };
        
    }
    
}

#endif // __SYSTEM_H__
