#include "KernelObject.h"
#include "Interrupts.h"
#include "KernelObject_Service.h"
#include "Process.h"

namespace KernelObject_Internal {
    class Service : public SystemService
    {
    protected:
        void Handle(UInt64 *parameters)
        {
            ::Handle handle = (::Handle)parameters[1];
            KernelObject *first = Process::Mapper()->Find(handle);
            if (!first) {
                parameters[0] = KERNELOBJECT_ERROR_INVALID_HANDLE;
                return;
            }
            switch (parameters[0]) {
                case KERNELOBJECT_FUNCTION_ADDREF:
                    if (!Process::Mapper()->MapAddRef(handle)) {
                        parameters[0] = KERNELOBJECT_ERROR_MEMORY_VIOLATION;
                        return;
                    }
                    break;
                case KERNELOBJECT_FUNCTION_RELEASE:
                    if (!Process::Mapper()->MapRelease(handle)) {
                        parameters[0] = KERNELOBJECT_ERROR_MEMORY_VIOLATION;
                        return;
                    }
                    break;
                case KERNELOBJECT_FUNCTION_HASH:
                    parameters[1] = first->Hash();
                    break;
                case KERNELOBJECT_FUNCTION_EQUAL:
                {
                    KernelObject *second = Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!second) {
                        parameters[0] = KERNELOBJECT_ERROR_INVALID_HANDLE_2;
                        return;
                    }
                    parameters[1] = first->IsEqualTo(second) ? 1 : 0;
                }
                    break;
                case KERNELOBJECT_FUNCTION_GETNAME:
                {
                    const char *name = first->GetClassName((UInt32)parameters[2]);
                    if (parameters[3]) {
                        int i = 0;
                        char *output = (char*)parameters[3];
                        while ((name[i] != '\0') && (i < parameters[4])) {
                            output[i] = name[i];
                            i++;
                        }
                        if (i < parameters[4])
                            output[i] = '\0';
                        parameters[1] = i;
                    } else {
                        parameters[1] = StringLength(name);
                    }
                }
                    break;
                default:
                    parameters[0] = KERNELOBJECT_ERROR_BAD_FUNCTION;
                    return;
            }
            parameters[0] = KERNELOBJECT_ERROR_SUCCESS;
        }
        
        int Interrupt(void)
        {
            return KERNELOBJECT_SYSCALL;
        }
    };
}

static KernelObject_Internal::Service service;

void KernelObject::ConfigureService(void)
{
    service.Register();
}
