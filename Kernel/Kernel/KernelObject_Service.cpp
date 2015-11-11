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
            KernelObject *first = Process::Mapper()->Find((::Handle)parameters[1]);
            if (!first) {
                parameters[0] = KERNELOBJECT_ERROR_INVALID_HANDLE;
                return;
            }
            switch (parameters[0]) {
                case KERNELOBJECT_FUNCTION_ADDREF:
                    first->AddRef();
                    break;
                case KERNELOBJECT_FUNCTION_RELEASE:
                    first->Release();
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
                    const char *name = first->GetClassName((UInt32)parameters[1]);
                    if (parameters[3]) {
                        int i = 0;
                        char *output = (char*)parameters[2];
                        while ((name[i] != '\0') && (i < parameters[3])) {
                            output[i] = name[i];
                            i++;
                        }
                        if (i < parameters[3])
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
