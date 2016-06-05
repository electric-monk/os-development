#include "Collections.h"
#include "Handle_Internal.h"
#include "../../Kernel/Kernel/Collections_Service.h"

namespace Kernel {

    namespace Collections {
        
        String* String::Create(const char *str, int length)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_CREATE_STRING), "b"((int)str), "c"(length)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_CREATE_STRING, result);
            return (String*)handle;
        }
        
        int String::Length(void)
        {
            int result;
            int length;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(length)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_STRING_LENGTH), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_STRING_LENGTH, result);
            return length;
        }
        
        int String::Copy(char *outBuffer, int bufferLength)
        {
            int result;
            int length;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(length)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_STRING_GET), "b"((int)this), "c"((int)outBuffer), "d"(bufferLength)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_STRING_GET, result);
            return length;
        }
        
        Number* Number::Create(int value)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_CREATE_NUMBER), "b"(value)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_CREATE_NUMBER, result);
            return (Number*)handle;
        }
        
        int Number::Value(void)
        {
            int result;
            int value;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(value)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_NUMBER_GET), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_NUMBER_GET, result);
            return value;
        }
        
        void Number::Reset(int value)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_NUMBER_SET), "b"((int)this), "c"(value)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_NUMBER_SET, result);
        }
        
        Array* Create(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_CREATE_ARRAY)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_CREATE_ARRAY, result);
            return (Array*)handle;
        }
        
        void Array::Add(Handle *object)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_ADD), "b"((int)this), "c"((int)object)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_ADD, result);
        }
        
        void Array::Remove(Handle *object)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_REMOVE), "b"((int)this), "c"((int)object)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_REMOVE, result);
        }
        
        int Array::Find(Handle *object)
        {
            int result;
            int index;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(index)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_FIND), "b"((int)this), "c"((int)object)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_FIND, result);
            return index;
        }
        
        int Array::Count(void)
        {
            int result;
            int count;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(count)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_COUNT), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_COUNT, result);
            return count;
        }
        
        Handle* Array::ObjectAt(int index)
        {
            int result;
            int object;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(object)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_GET), "b"((int)this), "c"(index)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_GET, result);
            return (Handle*)object;
        }
        
        void Array::AddArray(Array *array)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_ARRAY_ADDARRAY), "b"((int)this), "c"((int)array)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_ARRAY_ADDARRAY, result);
        }
        
        Dictionary* Dictionary::Create(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_CREATE_DICT)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_CREATE_DICT, result);
            return (Dictionary*)handle;
        }
        
        void Dictionary::Set(Handle *key, Handle *value)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_DICT_SET), "b"((int)this), "c"((int)key), "d"((int)value)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_DICT_SET, result);
        }
        
        void Dictionary::Reset(Handle *key)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_DICT_CLEAR), "b"((int)this), "c"((int)key)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_DICT_CLEAR, result);
        }
        
        Handle* Dictionary::ObjectFor(Handle *key)
        {
            int result;
            int object;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(object)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_DICT_GET), "b"((int)this), "c"((int)key)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_DICT_GET, result);
            return (Handle*)object;
        }
        
        Array* Dictionary::AllKeys(void)
        {
            int result;
            int object;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(object)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_DICT_GET_KEYS), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_DICT_GET_KEYS, result);
            return (Array*)object;
        }
        
        Array* Dictionary::AllValues(void)
        {
            int result;
            int object;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(object)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_DICT_GET_VALUES), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_DICT_GET_VALUES, result);
            return (Array*)object;
        }
        
        FIFO* FIFO::Create(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_CREATE_FIFO)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_CREATE_FIFO, result);
            return (FIFO*)handle;
        }
        
        void FIFO::Push(Handle *object)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_FIFO_PUSH), "b"((int)this), "c"((int)object)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_FIFO_PUSH, result);
        }
        
        Handle* FIFO::Pop(void)
        {
            int result;
            int object;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(object)
                          : /* inputs */ [interrupt]"N"(COLLECTIONS_SYSCALL), "a"(COLLECTIONS_FUNCTION_FIFO_POP), "b"((int)this)
                          : /* clobbers */);
            CheckResult(COLLECTIONS_SYSCALL, COLLECTIONS_FUNCTION_FIFO_POP, result);
            return (Handle*)object;
        }
        
    }

}
