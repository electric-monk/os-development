#ifndef __HANDLE_H__
#define __HANDLE_H__

namespace Kernel {

    class Handle
    {
    public:
        void AddRef(void);
        void Release(void);
        
        int Hash(void);
        bool IsEqual(Handle *other);
        
        int GetClassName(int superclassCount, char *outputBuffer, int maxLength);
        
    private:
        Handle();
        ~Handle();
    };

}

#endif // __HANDLE_H__
