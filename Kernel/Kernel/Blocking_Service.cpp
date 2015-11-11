#include "Interrupts.h"
#include "Blocking_Service.h"
#include "Process.h"
#include "Blocking.h"
#include "Thread.h"
#include "Collections.h"

namespace Blocking_Internal {
    class Service : public SystemService
    {
    protected:
        void Handle(UInt64 *parameters)
        {
            switch (parameters[0]) {
                    
                    /* Creation functions */
                    
                case BLOCKING_FUNCTION_CREATE_OR:
                    parameters[1] = Process::Mapper()->Map(new SignalOr());
                    break;
                case BLOCKING_FUNCTION_CREATE_AND:
                    parameters[1] = Process::Mapper()->Map(new SignalAnd());
                    break;
                case BLOCKING_FUNCTION_CREATE_SIMPLE:
                    parameters[1] = Process::Mapper()->Map(new SimpleSignal(parameters[1]));
                    break;
                case BLOCKING_FUNCTION_CREATE_TIMER:
                    parameters[1] = Process::Mapper()->Map(new Timer());
                    break;
                    
                    /* Timer functions */
                    
                case BLOCKING_FUNCTION_TIMER_RESET:
                {
                    Timer *timer = (Timer*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!timer || !timer->IsDerivedFromClass("Timer")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    timer->Reset((UInt32)parameters[2], parameters[3]);
                }
                    break;
                case BLOCKING_FUNCTION_TIMER_CANCEL:
                {
                    Timer *timer = (Timer*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!timer || !timer->IsDerivedFromClass("Timer")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    timer->Cancel();
                }
                    break;
                    
                    /* List (AND/OR) functions */
                    
                case BLOCKING_FUNCTION_ADD:
                {
                    ListSignalWatcher *watcher = (ListSignalWatcher*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!watcher || !watcher->IsDerivedFromClass("ListSignalWatcher")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    BlockableObject *blocker = (BlockableObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!blocker || !blocker->IsDerivedFromClass("BlockableObject")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    watcher->AddSource(blocker);
                }
                    break;
                case BLOCKING_FUNCTION_REMOVE:
                {
                    ListSignalWatcher *watcher = (ListSignalWatcher*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!watcher || !watcher->IsDerivedFromClass("ListSignalWatcher")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    BlockableObject *blocker = (BlockableObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!blocker || !blocker->IsDerivedFromClass("BlockableObject")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    watcher->RemoveSource(blocker);
                }
                    break;
                    
                    /* Simple signal */
                    
                case BLOCKING_FUNCTION_SIMPLE_SET:
                {
                    SimpleSignal *signal = (SimpleSignal*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!signal || !signal->IsDerivedFromClass("SimpleSignal")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    if (parameters[2])
                        signal->Set();
                    else
                        signal->Reset();
                }
                    
                    /* Actual blocking */
                    
                case BLOCKING_FUNCTION_BLOCK:
                {
                    BlockableObject *blocker = (BlockableObject*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!blocker || !blocker->IsDerivedFromClass("BlockableObject")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *result = Thread::Active->BlockOn(blocker);
                    if (result) {
                        result->AddRef();   // Ownership moves on to userspace
                        parameters[1] = Process::Mapper()->Map(result);
                    } else {
                        // TODO: Somethinghere
                    }
                }
                    break;
                    
                    /* Blocking object */
                    
                case BLOCKING_FUNCTION_IS_BLOCKED:
                {
                    BlockableObject *blocker = (BlockableObject*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!blocker || !blocker->IsDerivedFromClass("BlockableObject")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = blocker->IsSignalled() ? 1 : 0;
                }
                    break;
                case BLOCKING_FUNCTION_LIST:
                {
                    BlockableObject *blocker = (BlockableObject*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!blocker || !blocker->IsDerivedFromClass("BlockableObject")) {
                        parameters[0] = BLOCKING_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *array = blocker->CurrentSignals();
                    array->AddRef();
                    parameters[1] = Process::Mapper()->Map(array);
                }
                    break;
                    
                default:
                    parameters[0] = BLOCKING_ERROR_BAD_FUNCTION;
                    return;
            }
            parameters[0] = BLOCKING_ERROR_SUCCESS;
        }
        
        int Interrupt(void)
        {
            return BLOCKING_SYSCALL;
        }
    };
}
