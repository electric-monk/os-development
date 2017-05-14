#ifndef __ELFSYMS_H__
#define __ELFSYMS_H__

#include "Collections.h"

class ElfProvider
{
public:
    virtual UInt64 Header(void) = 0;
    virtual UInt64 Count(void) = 0;
    virtual UInt64 Strndx(void) = 0;
    virtual void* GetAddress(UInt64 address) = 0;
};

class ElfSymbols : public KernelObject
{
public:
    class Symbol : public KernelObject
    {
    public:
        Symbol(UInt64 base, UInt64 len, UInt64 nameIndex);
        void LoadName(const char *strings);
        
        KernelString* Name(void) const { return _name; }
        void* Base(void) const { return (void*)_base; }
        UInt64 Length(void) const { return _len; }
        
        UInt64 Offset(void *addr);
        
    protected:
        ~Symbol();
        
    private:
        UInt64 _nameIndex;
        KernelString *_name;
        UInt64 _base, _len;
    };
    
    ElfSymbols();
    
    int Parse(ElfProvider *source);
    
    Symbol* Find(void *address);
    
protected:
    ~ElfSymbols();
    
private:
    KernelArray *_symbols;
};

#endif // __ELFSYMS_H__
