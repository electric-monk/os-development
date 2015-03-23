#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__

#include "KernelObject.h"

class KernelArray;
class KernelArrayItem;
class HashEntry;
class FIFOItem;

#define KS(s)       KernelString::Create(s)

class KernelFIFO : public KernelObject
{
public:
    KernelFIFO();
    
    void Push(KernelObject *object);
    KernelObject* Pop(void);
    
protected:
    ~KernelFIFO();
    
private:
    FIFOItem *_start, *_end;
};

class KernelDictionary : public KernelObject
{
public:
    KernelDictionary();
    
    void Set(KernelObject *key, KernelObject *value);
    KernelObject* ObjectFor(KernelObject *key);
    
    KernelArray* AllKeys(void);
    KernelArray* AllObjects(void);

    UInt32 Hash(void);

protected:
    ~KernelDictionary();
    
private:
    UInt32 _slotCount;
    HashEntry *_slots;
    
    void Resize(UInt32 size);
};

class KernelArray : public KernelObject
{
public:
	static const UInt32 InvalidIndex;
    
    KernelArray();
    
    void Add(KernelObject *object);
    void Remove(KernelObject *object);
    bool Contains(KernelObject *object);
    UInt32 Find(KernelObject *object);
    
    void AddArray(KernelArray *other);
    void Sort(int (*compare)(KernelObject*,KernelObject*));
    
    UInt32 Count(void);
    KernelObject* ObjectAt(UInt32 index);
    
    UInt32 Hash(void);
    
protected:
    ~KernelArray();
    
private:
    KernelObject **_index;
    UInt32 _max;
    UInt32 _count;
    
    void CheckCapacity(UInt32 required);
};

class KernelString : public KernelObject
{
public:
    static KernelString* Create(const char *input);
    static KernelString* Format(const char *input, ...);
    KernelString(const char *input);
    
    const char *CString(void);
    UInt32 Length(void);
    
    UInt32 Hash(void);
    
protected:
    KernelString(char *input, UInt32 length);
    ~KernelString();
    
private:
    char *_data;
    UInt32 _length;
};

class KernelNumber : public KernelObject
{
public:
    static KernelNumber* Create(UInt32 value);
    
    KernelNumber(UInt32 value) { _value = value; }
    
    UInt32 Value(void) { return _value; }
    
    UInt32 Hash(void) { return _value; }
    
    void Reset(UInt32 newValue) { _value = newValue; }
    
private:
    UInt32 _value;
};

#endif // __COLLECTIONS_H__
