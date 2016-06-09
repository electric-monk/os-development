#include "System_Service.h"
#include "Interrupts.h"
#include "Process.h"
#include "Collections.h"
#include "Blocking.h"
#include "Driver.h"

extern Driver *s_rootDevice;

namespace System_Internal {
    class Monitor : public BlockableObject
    {
    public:
        CLASSNAME(BlockableObject, System_Internal::Monitor);
        
        Monitor()
        {
            
        }
        
        KernelArray* Capture(void)
        {
            KernelArray *result = new KernelArray();
            _providers->Enumerate([result](KernelObject *provider){
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
                return (void*)NULL;
            });
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
                case SYSTEM_DRIVER_GET_ROOT:
                    parameters[1] = Process::Mapper()->Map(s_rootDevice);
                    s_rootDevice->AddRef(); // For user app
                    break;
                case SYSTEM_DRIVER_CHILDREN:
                {
                    Driver *driver = (Driver*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!driver || !driver->IsDerivedFromClass("Driver")) {
                        parameters[0] = SYSTEM_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *array = new KernelArray();
                    Driver *child;
                    int i = 0;
                    while ((child = driver->Child(i++)) != NULL)
                        array->Add(child);
                    parameters[1] = Process::Mapper()->Map(array);
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

static System_Internal::Service service;

void Driver::ConfigureService(void)
{
    service.Register();
}
