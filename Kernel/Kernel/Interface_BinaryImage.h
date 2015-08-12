#ifndef __INTERFACE_BINARYIMAGE_H__
#define __INTERFACE_BINARYIMAGE_H__

#include "Interface.h"

namespace Interface_BinaryImage {

    /* REQUESTS - requests that can be sent to these services */

    class Request : public Interface_Request
    {
    public:
        static const UInt32 GetChunks = Interface_Request::MAX + 0; // Request -> GotChunks
        static const UInt32 ReadChunk = Interface_Request::MAX + 1; // Read -> ReadChunk
        static const UInt32 GetSymbols = Interface_Request::MAX + 2;// Request -> GotSymbols
        static const UInt32 MAX = Interface_Request::MAX + 100;
    };
    
    class Read : public Request
    {
    public:
        UInt32 chunk;
        UInt64 offset, length;
    };
    
    /* RESPONSES - responses that will be received from these services */

    class Response : public Interface_Response
    {
    public:
        static const UInt32 InvalidChunk = Interface_Response::MAX + 0;
        static const UInt32 MAX = Interface_Response::MAX + 100;
    };
    
    class GotChunks : public Response
    {
    public:
        class Chunk
        {
        public:
            UInt64 virtualAddress;
            UInt64 length;
            // flags
            bool read, write, executable;
        };
        
        UInt32 chunkCount;
        Chunk chunks[];
    };
    
    class ReadChunk : public Response
    {
    public:
        class RLE
        {
        public:
            typedef enum {
                eUnspecified,
                eZero,
                eNormal,
                eEnd,
            } Encoding;
            
            UInt32 length;
            Encoding encoding;
            
            UInt8* Data(void)
            {
                if (encoding != eNormal)
                    return NULL;
                return ((UInt8*)this) + sizeof(*this);
            }
            
            UInt64 Populate(UInt8 *buffer)
            {
                switch (encoding) {
                    default:
                        break;
                    case eZero:
                        for (UInt64 i = 0; i < length; i++)
                            buffer[i] = '\0';
                        break;
                    case eNormal: {
                        UInt8 *data = Data();
                        for (UInt64 i = 0; i < length; i++)
                            buffer[i] = data[i];
                        break;
                    }
                }
                return length;
            }
            
            RLE* Next(void)
            {
                UInt32 extra = (encoding == eNormal) ? length : 0;
                return (RLE*)(((UInt8*)this) + sizeof(*this) + extra);
            }
        };
        RLE chunkStart;
        void Populate(UInt8 *output)
        {
            RLE *cursor = &chunkStart;
            while (cursor->encoding != RLE::eEnd) {
                output += cursor->Populate(output);
                cursor = cursor->Next();
            }
        }
    };
    
// All fields optional for symbols dictionaries:
#define Symbol_Name             "name"      /* FlatString - default none */
#define Symbol_Address_Virtual  "virtual"   /* FlatInteger - default none */
#define Symbol_Launch           "launch"    /* FlatInteger - default 0: if non-zero, entrypoint is a main-like symbol, and should be attached to a thread on load of the binary */
    
    class GotSymbols : public Response
    {
    public:
        FlatArray symbols;
            // FlatDictionary: values
    };
}

#endif // __INTERFACE_BINARYIMAGE_H__
