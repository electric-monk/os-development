#include "Interrupts.h"
#include "Collections_Service.h"
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
                default:
                    parameters[0] = COLLECTIONS_ERROR_BAD_FUNCTION;
                    return;
                    
                case COLLECTIONS_FUNCTION_CREATE_ARRAY:
                    parameters[1] = Process::Mapper()->Map(new KernelArray());
                    break;
                case COLLECTIONS_FUNCTION_CREATE_DICT:
                    parameters[1] = Process::Mapper()->Map(new KernelDictionary());
                    break;
                case COLLECTIONS_FUNCTION_CREATE_FIFO:
                    parameters[1] = Process::Mapper()->Map(new KernelFIFO());
                    break;
                case COLLECTIONS_FUNCTION_CREATE_NUMBER:
                    parameters[1] = Process::Mapper()->Map(new KernelNumber((UInt32)parameters[1]));
                    break;
                case COLLECTIONS_FUNCTION_CREATE_STRING:
                    parameters[1] = Process::Mapper()->Map(new KernelString((char*)parameters[1], (UInt32)parameters[2]));
                    break;
                    
                    /* Strings */
                    
                case COLLECTIONS_FUNCTION_STRING_GET:
                {
                    KernelString *string = (KernelString*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!string || !string->IsDerivedFromClass("KernelString")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    UInt32 length = string->Length();
                    if (length > parameters[3])
                        length = (UInt32)parameters[3];
                    CopyMemory((void*)parameters[2], string->CString(), length);
                    parameters[1] = length;
                }
                    break;
                case COLLECTIONS_FUNCTION_STRING_LENGTH:
                {
                    KernelString *string = (KernelString*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!string || !string->IsDerivedFromClass("KernelString")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = string->Length();
                }
                    break;
                    
                    /* Numbers */
                    
                case COLLECTIONS_FUNCTION_NUMBER_GET:
                {
                    KernelNumber *number = (KernelNumber*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!number || !number->IsDerivedFromClass("KernelNumber")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = number->Value();
                }
                    break;
                case COLLECTIONS_FUNCTION_NUMBER_SET:
                {
                    KernelNumber *number = (KernelNumber*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!number || !number->IsDerivedFromClass("KernelNumber")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    number->Reset((UInt32)parameters[1]);
                }
                    break;

                    /* FIFO */
                    
                case COLLECTIONS_FUNCTION_FIFO_PUSH:
                {
                    KernelFIFO *fifo = (KernelFIFO*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!fifo || !fifo->IsDerivedFromClass("KernelFIFO")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *object = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!object) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    fifo->Push(object);
                }
                    break;
                case COLLECTIONS_FUNCTION_FIFO_POP:
                {
                    KernelFIFO *fifo = (KernelFIFO*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!fifo || !fifo->IsDerivedFromClass("KernelFIFO")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *result = fifo->Pop();
                    if (!result) {
                        parameters[0] = COLLECTIONS_ERROR_FIFO_EMPTY;
                        return;
                    }
                    result->AddRef();
                    parameters[1] = Process::Mapper()->Map(result);
                }
                    break;
                    
                    /* Dictionary */
                    
                case COLLECTIONS_FUNCTION_DICT_SET:
                {
                    KernelDictionary *dictionary = (KernelDictionary*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!dictionary || !dictionary->IsDerivedFromClass("KernelDictionary")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *key = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!key) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *value = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[3]);
                    if (!value) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    dictionary->Set(key, value);
                }
                    break;
                case COLLECTIONS_FUNCTION_DICT_GET:
                {
                    KernelDictionary *dictionary = (KernelDictionary*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!dictionary || !dictionary->IsDerivedFromClass("KernelDictionary")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *key = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!key) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *value = dictionary->ObjectFor(key);
                    if (!value) {
                        parameters[0] = COLLECTIONS_ERROR_NOT_FOUND;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(value);
                }
                    break;
                case COLLECTIONS_FUNCTION_DICT_CLEAR:
                {
                    KernelDictionary *dictionary = (KernelDictionary*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!dictionary || !dictionary->IsDerivedFromClass("KernelDictionary")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *key = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!key) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    dictionary->Set(key, NULL);
                }
                    break;
                case COLLECTIONS_FUNCTION_DICT_GET_KEYS:
                {
                    KernelDictionary *dictionary = (KernelDictionary*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!dictionary || !dictionary->IsDerivedFromClass("KernelDictionary")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *result = dictionary->AllKeys();
                    result->AddRef();
                    parameters[1] = Process::Mapper()->Map(result);
                }
                    break;
                case COLLECTIONS_FUNCTION_DICT_GET_VALUES:
                {
                    KernelDictionary *dictionary = (KernelDictionary*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!dictionary || !dictionary->IsDerivedFromClass("KernelDictionary")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *result = dictionary->AllObjects();
                    result->AddRef();
                    parameters[1] = Process::Mapper()->Map(result);
                }
                    break;
                    
                    /* Array */
                    
                case COLLECTIONS_FUNCTION_ARRAY_ADD:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *object = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!object) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    array->Add(object);
                }
                    break;
                case COLLECTIONS_FUNCTION_ARRAY_ADDARRAY:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelArray *otherArray = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!otherArray) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    array->AddArray(otherArray);
                }
                    break;
                case COLLECTIONS_FUNCTION_ARRAY_COUNT:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = array->Count();
                }
                    break;
                case COLLECTIONS_FUNCTION_ARRAY_FIND:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *object = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!object) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = array->Find(object);
                }
                    break;
                case COLLECTIONS_FUNCTION_ARRAY_GET:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *result = array->ObjectAt((UInt32)parameters[2]);
                    if (!result) {
                        parameters[0] = COLLECTIONS_ERROR_NOT_FOUND;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(result);
                }
                    break;
                case COLLECTIONS_FUNCTION_ARRAY_REMOVE:
                {
                    KernelArray *array = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!array || !array->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelObject *object = (KernelObject*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!object) {
                        parameters[0] = COLLECTIONS_ERROR_INVALID_HANDLE;
                        return;
                    }
                    array->Remove(object);
                }
                    break;
            }
            parameters[0] = COLLECTIONS_ERROR_SUCCESS;
        }
        
        int Interrupt(void)
        {
            return COLLECTIONS_SYSCALL;
        }
    };
}
