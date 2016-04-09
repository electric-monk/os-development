#include "System_Service.h"
#include "Interrupts.h"
#include "Process.h"
#include "Collections.h"

namespace System_Internal {
    class Monitor : public KernelObject
    {
    public:
        CLASSNAME(KernelObject, System_Internal::Monitor);
        
        Monitor()
        {
            
        }
        
        KernelArray* Capture(void)
        {
            KernelArray *result = new KernelArray();
            int i, j;
            j = _providers->Count();
            for (i = 0; i < j; i++) {
                
                KernelDictionary *providerEntry = new KernelDictionary();
                providerEntry->Set("object"_ko, provider);
                KernelArray *inputs = new KernelArray();
                providerEntry->Set("inputs"_ko, inputs);
                inputs->Release();
                KernelArray *outputs = new KernelArray();
                providerEntry->Set("outputs"_ko, outputs);
                outputs->Release();
                result->Add(providerEntry);
                providerEntry->Release();
            }
            return result;
        }
        
    protected:
        ~Monitor()
        {
            
        }
        
    private:
        KernelArray *_providers;
    };
    
    class Service : public SystemService
    {
    public:
        void Handle(UInt64 *parameters)
        {
            switch (parameters[0]) {
                case SYSTEM_MONITOR_CREATE:
                    parameters[1] = Process::Mapper()->Map(new Monitor());
                    break;
                case SYSTEM_MONITOR_CAPTURE:
                {
                    Monitor *monitor = (Monitor*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!monitor || !monitor->IsDerivedFromClass("System_Internal::Monitor")) {
                        parameters[0] = SYSTEM_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(monitor->Capture());
                }
                    break;
                default:
                    parameters[0] = SYSTEM_ERROR_BAD_FUNCTION;
                    return;
            }
            parameters[0] = SYSTEM_ERROR_SUCCESS;
        }
        
        int Interrupt(void)
        {
            return SYSTEM_SYSCALL;
        }
    };
};
