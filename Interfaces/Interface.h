#ifndef __INTERFACE_H__
#define __INTERFACE_H__

/* BASE PACKET - since packets can be generated unsolicited, this is a base class */

class Interface_Packet
{
public:
    // Classes
    static const int Request = 0;
    static const int Response = 1;
    static const int Event = 2;
    static const int MAX = 100;

    UInt32 packetClass;
    
    // Useful fields
    UInt32 identifier;
    UInt32 type;
};

/* REQUESTS - requests that can be sent to these services */

class Interface_Request : public Interface_Packet
{
public:
    // Standard interface requests
    static const int MAX = 100;
    
};

/* RESPONSES - responses that will be received from these services */

class Interface_Response : public Interface_Packet
{
public:
    // Standard response statuses
    static const int Success = 0;
    static const int Unsupported = 1;
    static const int MAX = 100;
    
    void Fill(Interface_Request *request)
    {
        packetClass = Interface_Packet::Response;
        identifier = request->identifier;
        type = request->type;
    }
    UInt32 status;
};

/* UTILITIES - classes that can be used to encode requests */

inline UInt32 operator"" _type(char const *x, size_t len)
{
    return
        (x[0] << 24)
    |
        (x[1] << 16)
    |
        (x[2] << 8)
    |
        x[3];
}

class FlatObject
{
protected:
    static UInt32 EndType(void) { return "End!"_type; }
public:
    UInt32 length;      // Total length of this flat object
    UInt32 type;        // Type identifier
    FlatObject* NextObject(bool writing = false)   // Returns the pointer in the buffer after this one, whether or not it's a real object
    {
        // Compute next address
        UInt64 addr = UInt64(this) + length;
        // Make it 32-bit aligned
        UInt64 extra = addr & 0x03;
        if (extra)
            addr += 4 - extra;
        // Is it End?
        FlatObject *next = (FlatObject*)addr;
        if (!writing && ((next->length == 0) || (next->type == EndType())))
            return NULL;
        return next;
    }
    bool IsEqual(FlatObject *other)
    {
        // By comparing bytes, we'll implicitly check type and size too, so it's all good
        char *a = (char*)this, *b = (char*)other;
        for (UInt32 i = 0; i < length; i++, a++, b++)
            if (*a != *b)
                return false;
        return true;
    }
    void ReleaseDynamic(void)
    {
        delete[] (char*)this;
    }
    void CopyFrom(FlatObject *source)
    {
        char *a = (char*)this, *b = (char*)source;
        for (UInt32 i = 0; i < source->length; i++, a++, b++)
            *a = *b;
    }
protected:
    static FlatObject* AllocDynamic(UInt32 length, UInt32 type)
    {
        length += sizeof(FlatObject);
        FlatObject *obj = (FlatObject*)new char[length];
        obj->length = length;
        obj->type = type;
        return obj;
    }
};

class FlatEnd : public FlatObject
{
public:
    static UInt32 Type(void) { return FlatObject::EndType(); }
    
    // Use this class at the end of a buffer - if you have a non-array list of objects, it'll ensure NextObject(false) returns NULL at the end
    void Initialise(void)
    {
        length = 0;
        type = Type();
    }
};

class FlatBlob : public FlatObject
{
public:
    static UInt32 Type(void) { return "Blob"_type; }
    
    // For writing
    void Initialise(const void *data, UInt32 length)
    {
        type = Type();
        this->length = sizeof(FlatBlob) + length;
        char *input = (char*)data;
        char *output = (char*)Data();
        for (UInt32 i = 0; i < length; i++, input++, output++)
            *output = *input;
    }
    
    // For reading
    UInt32 Length(void)
    {
        return length - sizeof(FlatBlob);
    }
    const void* Data(void)
    {
        return ((char*)this) + sizeof(FlatBlob);
    }
};

class FlatString : public FlatBlob
{
public:
    static UInt32 Type(void) { return "Strn"_type; }
    
    static FlatString* CreateDynamic(const char *str)
    {
        UInt32 length;
        const char *s;
        for (s = str, length = 0; *s != '\0'; s++, length++);
        length++;   // for NUL
        FlatString *obj = (FlatString*)AllocDynamic(length, Type());
        char *output = (char*)obj->Value();
        s = str;
        for (UInt32 i = 0; i < length; i++, output++, s++)
            *output = *s;
        return obj;
    }
    
    // For writing
    void Initialise(const char *str)
    {
        UInt32 count;
        for (count = 0; str[count] != '\0'; count++);
        FlatBlob::Initialise(str, count + 1);
        type = Type();
    }
    
    // For reading
    UInt32 Length(void)
    {
        return FlatBlob::Length() - 1;
    }
    const char *Value(void)
    {
        return (const char*)Data();
    }
};

class FlatInteger : public FlatObject
{
private:
    UInt64 _value;
public:
    static UInt32 Type(void) { return "Intg"_type; }
    
    static FlatInteger* CreateDynamic(UInt64 value)
    {
        FlatInteger *obj = (FlatInteger*)AllocDynamic(sizeof(FlatInteger), Type());
        obj->_value = value;
        return obj;
    }
    
    // For writing
    void Initialise(UInt64 value)
    {
        type = Type();
        length = sizeof(FlatInteger);
        _value = value;
    }
    
    // For reading
    UInt64 Value(void)
    {
        return _value;
    }
};

class FlatArray : public FlatObject
{
private:
    UInt32 _count;
protected:
    FlatObject* Start(bool writing)
    {
        if (!writing && _count == 0)
            return NULL;
        return (FlatObject*)(((char*)this) + sizeof(FlatArray));
    }
public:
    static UInt32 Type(void) { return "Arry"_type; }
    
    // For writing
    void Initialise(void)
    {
        type = Type();
        length = sizeof(FlatArray);
        _count = 0;
    }
    FlatObject* GetNextAddress(void)
    {
        FlatObject *object = Start(true);
        for (UInt32 cur = _count; cur != 0; cur--)
            object = object->NextObject(true);
        return object;
    }
    void CompleteNextItem(void)    // After placing an object at GetNextAddress()
    {
        FlatObject *next = GetNextAddress();
        // Doing all this takes into account any alignment imposed by GetNextAddress()
        char* nextStart = (char*)next;
        char* nextEnd = nextStart + next->length;
        length += nextEnd - (((char*)this) + length);
        _count++;
    }
    
    // For reading
    UInt32 Count(void)
    {
        return _count;
    }
    FlatObject* ItemAt(UInt32 index)
    {
        if (index >= _count)
            return NULL;
        FlatObject *object = Start(false);
        for (UInt32 cur = index; cur != 0; cur--)
            object = object->NextObject(false);
        return object;
    }
};

class FlatDictionary : public FlatArray
{
public:
    static UInt32 Type(void) { return "Dict"_type; }
    
    // For writing
    void Initialise(void)
    {
        FlatArray::Initialise();
        type = Type();
    }
    // Write key, then value, as above
    
    // For reading
    FlatObject* ItemFor(FlatObject *key)
    {
        FlatObject *cursor = Start(false);
        while (cursor != NULL) {
            FlatObject *next = cursor->NextObject(false);
            if (cursor->IsEqual(key))
                return next;
            cursor = next ? next->NextObject(false) : NULL;
        }
        return NULL;
    }
};

class FlatDate : public FlatObject
{
public:
    static UInt32 Type(void) { return "Date"_type; }
    
    UInt32 Year, Month, Day, Hour, Minute, Second, Millisecond;
    
    void Initialise(void)
    {
        type = Type();
        length = sizeof(FlatDate);
    }
};

#endif // __INTERFACE_H__
