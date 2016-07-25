#include "System_Service.h"
#include "Interrupts.h"
#include "Process.h"
#include "Collections.h"
#include "Blocking.h"
#include "Driver.h"
#include "IPC_Manager.h"

extern Driver *s_rootDevice;

namespace System_Internal {
    
    class Service : public SystemService
    {
    public:
        void Handle(UInt64 *parameters)
        {
            AutoreleasePool releaser;
            switch (parameters[0]) {
                case SYSTEM_MONITOR_CREATE:
                    parameters[1] = Process::Mapper()->Map(new IpcServiceMonitor());
                    break;
                case SYSTEM_MONITOR_CAPTURE:
                {
                    IpcServiceMonitor *monitor = (IpcServiceMonitor*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!monitor || !monitor->IsDerivedFromClass("IpcServiceMonitor")) {
                        parameters[0] = SYSTEM_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *changes = monitor->Changes();
                    changes->AddRef();
                    parameters[1] = Process::Mapper()->Map(changes);
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
