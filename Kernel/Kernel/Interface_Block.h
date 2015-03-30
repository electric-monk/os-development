#ifndef __INTERFACE_BLOCK_H__
#define __INTERFACE_BLOCK_H__

/* SERVICES - the services defined by this file */

#define SERVICE_TYPE_BLOCK          "core.provider.block"

/* REQUESTS - requests that can be sent to these services */

class BlockRequest
{
public:
    typedef enum {
        blockRequestRead,
        blockRequestWrite,
        blockRequestEject,
    } BlockRequestType;
    
    UInt64 identifier;
    BlockRequestType type;
};

class BlockRequestRead : public BlockRequest
{
public:
    UInt64 offset;
    UInt64 length;
};

class BlockRequestWrite : public BlockRequest
{
public:
    UInt64 offset;
    UInt64 length;
    char* data(void) { return ((char*)this) + sizeof(*this); }
};

/* RESPONSES - responses that will be received from these services */

class BlockResponse
{
public:
    typedef enum {
        blockResponseSuccess,
        blockResponseNotSupported,
        blockResponseGeneralFailure,
    } BlockResponseStatus;
    
    BlockResponseStatus status;
    BlockRequest originalRequest;   // comes last
};

class BlockResponseRead /*: public BlockResponse*/
{
public:
    BlockResponse::BlockResponseStatus status;
    BlockRequestRead originalRequest;
    UInt64 actualOffset;
    UInt64 actualLength;
    char* rawData(void) { return ((char*)this) + sizeof(*this); }
    char* data(void) { return rawData() + (actualOffset - originalRequest.offset); }
};

#endif // __INTERFACE_BLOCK_H__
