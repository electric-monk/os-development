#ifndef __INTERFACE_MOUSE_H__
#define __INTERFACE_MOUSE_H__

#include "Interface.h"

#define SERVICE_TYPE_MOUSE              "core.provider.mouse"_ko

namespace Interface_Mouse {
    
    /* REQUESTS - requests that can be sent to these services */
    
    class Command : public Interface_Request
    {
    public:
        static const UInt32 MAX = Interface_Request::MAX + 100;
    };

    /* RESPONSES - responses that will be received from these services */

    class Response : public Interface_Response
    {
    public:
    };
    
    /* NOTIFICATIONS - unsolicited messages from this source */
    
    class Event : public Interface_Packet
    {
    public:
        static const UInt32 Button = Interface_Packet::MAX + 0;
        static const UInt32 Motion = Interface_Packet::MAX + 1;
        static const UInt32 Wheel = Interface_Packet::MAX + 2;
        static const UInt32 MAX = Interface_Packet::MAX + 100;
        
        UInt32 timestamp;
    };
    
    class Button : public Event
    {
    public:
        UInt32 index;
        bool down;
    };
    
    class Motion : public Event
    {
    public:
        // Wheel or motion
        SInt32 x, y;
    };
}

#endif // __INTERFACE_MOUSE_H__
