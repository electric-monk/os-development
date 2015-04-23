#ifndef __INTERFACE_BLOCK_H__
#define __INTERFACE_BLOCK_H__

#include "Interface.h"

/* SERVICES - the services defined by this file */

#define SERVICE_TYPE_BLOCK          "core.provider.block"_ko

/* REQUESTS - requests that can be sent to these services */

class BlockRequest : public Interface_Request
{
public:
    static const int Read = Interface_Request::MAX + 0;
    static const int Write = Interface_Request::MAX + 1;
    static const int Eject = Interface_Request::MAX + 2;
    static const int MAX = Interface_Request::MAX + 100;
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

class BlockResponse : public Interface_Response
{
public:
    static const int GeneralFailure = Interface_Response::MAX + 0;
    static const int MAX = Interface_Response::MAX + 100;
};

class BlockResponseRead /*: public BlockResponse*/
{
public:
    // From BlockResponse
    UInt32 status;
    BlockRequestRead originalRequest;
    // Read response
    UInt64 actualOffset;
    UInt64 actualLength;
    char* rawData(void) { return ((char*)this) + sizeof(*this); }
    char* data(void) { return rawData() + (originalRequest.offset - actualOffset); }
    UInt64 length(void) { return actualLength - actualOffset; }
};

#endif // __INTERFACE_BLOCK_H__
