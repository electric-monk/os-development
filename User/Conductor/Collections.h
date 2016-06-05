#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__

#include "Handle.h"

namespace Kernel {

    namespace Collections {
        
        class String : public Handle
        {
        public:
            static String* Create(const char *str, int length);
            
            int Length(void);
            int Copy(char *outBuffer, int bufferLength);
        };

        class Number : public Handle
        {
        public:
            static Number* Create(int value);
            
            int Value(void);
            void Reset(int value);
        };
        
        class Array : public Handle
        {
        public:
            static Array* Create(void);
            
            void Add(Handle *object);
            void Remove(Handle *object);
            int Find(Handle *object);
            int Count(void);
            Handle* ObjectAt(int index);
            void AddArray(Array *array);
        };
        
        class Dictionary : public Handle
        {
        public:
            static Dictionary* Create(void);
            
            void Set(Handle *key, Handle *value);
            void Reset(Handle *key);
            Handle* ObjectFor(Handle *key);
            Array* AllKeys(void);
            Array* AllValues(void);
        };
        
        class FIFO : public Handle
        {
        public:
            static FIFO* Create(void);
            
            void Push(Handle *object);
            Handle* Pop(void);
        };
    
    }
    
}


#endif // __COLLECTIONS_H__
