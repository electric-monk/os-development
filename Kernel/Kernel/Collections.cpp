#include "KernelObject.h"
#include "Collections.h"
#include "tools.h"

/* FIFO */

class FIFOItem
{
public:
    FIFOItem(FIFOItem **start, FIFOItem **end, KernelObject *item)
    {
        _start = start;
        _end = end;
        _next = NULL;
        _previous = *_end;
        if (_previous == NULL)
            *_start = this;
        else
            _previous->_next = this;
        *_end = this;
        this->item = item;
        this->item->AddRef();
    }
    ~FIFOItem()
    {
        item->Release();
        if (_previous != NULL)
            _previous->_next = _next;
        if (_next != NULL)
            _next->_previous = _previous;
        if (*_start == this)
            *_start = _next;
        if (*_end == this)
            *_end = _previous;
    }
    
    KernelObject *item;
    
private:
    FIFOItem **_start, **_end;
    FIFOItem *_previous, *_next;
};

KernelFIFO::KernelFIFO()
{
    _start = NULL;
    _end = NULL;
}

KernelFIFO::~KernelFIFO()
{
    while (_start != NULL)
        delete _start;
}

void KernelFIFO::Push(KernelObject *object)
{
    new FIFOItem(&_start, &_end, object);
}

KernelObject* KernelFIFO::Pop(void)
{
    if (_end == NULL)
        return NULL;
    KernelObject *result = _end->item;
    result->AddRef();
    delete _end;
    result->Autorelease();
    return result;
}

/* Dictionary */

// TODO: Some sort of tree structure?

class Entry
{
public:
    KernelObject *key;
    KernelObject *value;
    
    Entry(Entry **start, Entry **end, KernelObject *key, KernelObject *value)
    {
        // Store new values
        this->key = key;
        this->key->AddRef();
        this->value = value;
        this->value->AddRef();
        // Connect to linked list
        _start = start;
        _end = end;
        _next = NULL;
        _previous = *_end;
        if (_previous == NULL)
            *_start = this;
        else
            _previous->_next = this;
        *_end = this;
    }
    
    ~Entry()
    {
        // Unlink
        if (_previous != NULL)
            _previous->_next = _next;
        if (_next != NULL)
            _next->_previous = _previous;
        if (*_start == this)
            *_start = _next;
        if (*_end == this)
            *_end = _previous;
        // Release the values
        key->Release();
        value->Release();
    }
    
    void Change(KernelObject *value)
    {
        this->value->Release();
        this->value = value;
        this->value->AddRef();
    }
    
    Entry *_next;
private:
    Entry **_start, **_end;
    Entry *_previous;
};

class HashEntry
{
public:
    Entry *objects;
    HashEntry()
    {
        objects = NULL;
        end = NULL;
    }
    ~HashEntry()
    {
        while (objects != NULL)
            delete objects;
    }
    Entry* Find(KernelObject *key)
    {
        for (Entry *item = objects; item != NULL; item = item->_next) {
            if (item->key->IsEqualTo(key))
                return item;
        }
        return NULL;
    }
    void Set(KernelObject *key, KernelObject *value)
    {
        Entry *entry = Find(key);
        if (entry == NULL) {
            if (value != NULL)
                new Entry(&objects, &end, key, value);
        } else {
            if (value == NULL)
                delete entry;
            else
                entry->Change(value);
        }
    }
private:
    Entry *end;
};

KernelDictionary::KernelDictionary()
{
    _slotCount = 10;
    _slots = new HashEntry[_slotCount];
}

KernelDictionary::~KernelDictionary()
{
    delete[] _slots;
}

void KernelDictionary::Set(KernelObject *key, KernelObject *value)
{
    UInt32 index = key->Hash() % _slotCount;
    _slots[index].Set(key, value);
}

KernelObject* KernelDictionary::ObjectFor(KernelObject *key)
{
    UInt32 index = key->Hash() % _slotCount;
    Entry *entry = _slots[index].Find(key);
    return entry ? entry->value : NULL;
}

KernelArray* KernelDictionary::AllKeys(void)
{
    KernelArray *output = new KernelArray();
    for (UInt32 i = 0; i < _slotCount; i++) {
        for (Entry *entry = _slots[i].objects; entry != NULL; entry = entry->_next)
            output->Add(entry->key);
    }
    output->Autorelease();
    return output;
}

KernelArray* KernelDictionary::AllObjects(void)
{
    KernelArray *output = new KernelArray();
    for (UInt32 i = 0; i < _slotCount; i++) {
        for (Entry *entry = _slots[i].objects; entry != NULL; entry = entry->_next)
            output->Add(entry->value);
    }
    output->Autorelease();
    return output;
}

void KernelDictionary::Resize(UInt32 size)
{
    HashEntry *newSlots = new HashEntry[size];
    for (UInt32 i = 0; i < _slotCount; i++) {
        for (Entry *entry = _slots[i].objects; entry != NULL; entry = entry->_next) {
            KernelObject *key = entry->key;
            UInt32 newIndex = key->Hash() % size;
            newSlots[newIndex].Set(key, entry->value);
        }
    }
    delete[] _slots;
    _slots = newSlots;
    _slotCount = size;
}

UInt32 KernelDictionary::Hash(void)
{
    UInt32 hash = 0;
    for (UInt32 i = 0; i < _slotCount; i++) {
        for (Entry *entry = _slots[i].objects; entry != NULL; entry = entry->_next) {
            hash ^= entry->key->Hash();
            hash ^= entry->value->Hash();
        }
    }
    return hash;
}

/* Array */

// TODO: What's the best solution here? Chunks of a page?

#define ARRAY_GRANULARITY           10

const UInt32 KernelArray::InvalidIndex = (UInt32)-1;

KernelArray::KernelArray()
{
    _max = ARRAY_GRANULARITY;
    _index = new KernelObject*[_max];
    _count = 0;
}

KernelArray::~KernelArray()
{
    for (UInt32 i = 0; i < _count; i++)
        _index[i]->Release();
    delete[] _index;
}

void KernelArray::CheckCapacity(UInt32 required)
{
    // TODO: Also handle reducing index if things go away for a while
    if (required > _max) {
        UInt32 newMax = ((required + ARRAY_GRANULARITY - 1) / ARRAY_GRANULARITY) * ARRAY_GRANULARITY;
        KernelObject **newIndex = new KernelObject*[newMax];
        CopyMemory(newIndex, _index, sizeof(KernelObject*) * _count);
        delete[] _index;
        _index = newIndex;
        _max = newMax;
    }
}

void KernelArray::Add(KernelObject *object)
{
    CheckCapacity(_count + 1);
    _index[_count] = object;
    object->AddRef();
    _count++;
}

void KernelArray::Remove(KernelObject *object)
{
    UInt32 index = Find(object);
    _index[index]->Release();
    _count--;
    CopyMemory(_index + index, _index + index + 1, _count - index);
}

UInt32 KernelArray::Find(KernelObject *object)
{
    for (UInt32 i = 0; i < _count; i++) {
        if (_index[i]->IsEqualTo(object))
            return i;
    }
    return KernelArray::InvalidIndex;
}

bool KernelArray::Contains(KernelObject *object)
{
    return Find(object) != KernelArray::InvalidIndex;
}

UInt32 KernelArray::Count(void)
{
    return _count;
}

KernelObject* KernelArray::ObjectAt(UInt32 index)
{
    return _index[index];
}

UInt32 KernelArray::Hash(void)
{
    UInt32 hash = 0;
    for (UInt32 i = 0; i < _count; i++)
        hash ^= _index[i]->Hash();
    return hash;
}

/* String */

KernelString* KernelString::Create(const char *input)
{
    KernelString *result = new KernelString(input);
    result->Autorelease();
    return result;
}

class KernelStringGenerator : public CStringPrint
{
public:
    KernelStringGenerator(UInt32 chunkSize = 32)
    {
        _chunk = chunkSize;
        Reset();
    }
    ~KernelStringGenerator()
    {
        if (_data != NULL)
            delete[] _data;
    }
    
    char* FinaliseString(UInt32 *outputLength)
    {
        if (_data == NULL) {
            *outputLength = 1;
            return (char*)"";
        }
        Check(_length + 1);
        _data[_length] = '\0';
        char *result = _data;
        *outputLength = _length;
        Reset();
        return _data;
    }
    
protected:
    void PrintOut(const char *data, int length)
    {
        Check(_length + length);
        for (int i = 0; i < length; i++)
            _data[_length + i] = data[i];
        _length += length;
    }
    
private:
    char *_data;
    UInt32 _total, _chunk;
    UInt32 _length;
    
    void Reset(void)
    {
        _data = NULL;
        _total = 0;
        _length = 0;
    }
    void Check(UInt32 required)
    {
        if (_total >= required)
            return;
        UInt32 desired = (required + (_chunk - 1)) / _chunk;
        UInt32 newTotal = desired * _chunk;
        char *newData = new char[newTotal];
        if (_data != NULL) {
            CopyMemory(newData, _data, _length);
            delete[] _data;
        }
        _data = newData;
        _total = newTotal;
    }
};

KernelString* KernelString::Format(const char *input, ...)
{
    KernelStringGenerator generator;
	va_list vl;
    
	va_start(vl, input);
	generator.PrintList(input, vl);
	va_end(vl);
    
    UInt32 length;
    char *output = generator.FinaliseString(&length);
    KernelString *result = new KernelString(output, length);
    result->Autorelease();
    return result;
}

static UInt32 tempstrlen(const char *input)
{
    const char *end = input;
    while (*end)
        end++;
    return end - input;
}

KernelString::KernelString(const char *input)
{
    _length = tempstrlen(input);
    _data = new char[_length + 1];
    CopyMemory(_data, input, _length + 1);
}

KernelString::KernelString(char *input, UInt32 length)
{
    _data = input;
    _length = length;
}

KernelString::~KernelString()
{
    delete[] _data;
}

const char *KernelString::CString(void)
{
    return _data;
}

UInt32 KernelString::Length(void)
{
    return _length;
}

UInt32 KernelString::Hash(void)
{
    UInt32 hash = 0;
    const unsigned char *s = (const unsigned char*)_data;
    while (*s != '\0') {
        hash = hash * 37 + *s;
        s++;
    }
    return hash;
}

/* Number */

KernelNumber* KernelNumber::Create(UInt32 value)
{
    KernelNumber *result = new KernelNumber(value);
    result->Autorelease();
    return result;
}
