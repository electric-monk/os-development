#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "Handle.h"
#include "Collections.h"
#include "Blocking.h"

namespace Kernel {
    
    namespace System {
    
        class Driver : public Handle
        {
        public:
            static Driver* GetRootDriver(void);
            Collections::Array* Children(void);
        };
        
        class Monitor : public Blocking::Blockable
        {
        public:
            static Monitor* Create(void);
            Collections::Array* Changes(void);
            Handle* ObjectForIdentifier(int identifier);
        };
    }
    
}

#endif // __SYSTEM_H__
