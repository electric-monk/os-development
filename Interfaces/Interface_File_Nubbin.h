#ifndef __INTERFACE_FILE_NUBBIN_H__
#define __INTERFACE_FILE_NUBBIN_H__

#include "Interface.h"

namespace Interface_File_Nubbin {
    
    /* REQUESTS - requests that can be sent to these services */
    
    class Command : public Interface_Request
    {
    public:
        static const UInt32 ExposeFile = Interface_Request::MAX + 0;        // Expose -> ExposeResponse
        static const UInt32 UnexposeFile = Interface_Request::MAX + 1;      // Unexpose -> Response
        static const UInt32 MAX = Interface_Request::MAX + 100;
    };

    class Expose : public Command
    {
    public:
        bool exclusive; // Only allow one app to connect
        bool autoUnexpose;  // Unexpose automatically when the last connection ends
        // These relate to the file Interface_File connection the nubbin is connected to
        UInt64 rootNode;
        FlatArray subpath; // Optional, each component of the path should be a single string entry, or integer node.
    };
    
    class Unexpose : public Command
    {
    public:
        FlatObject exposedFile; // Either FlatObject handle or FlatString connectionName
    };

    /* RESPONSES - responses that will be received from these services */
    
    class Response : public Interface_Response
    {
    public:
        static const int MAX = Interface_Response::MAX + 100;
    };
    
    class ExposeResponse : public Response
    {
    public:
        UInt64 handle;
        FlatString connectionName;  // New output connection name
    };
}

#endif // __INTERFACE_FILE_NUBBIN_H__
