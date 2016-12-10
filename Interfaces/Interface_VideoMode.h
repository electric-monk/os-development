#ifndef __INTERFACE_VIDEOMODE_H__
#define __INTERFACE_VIDEOMODE_H__

#include "Interface.h"

#define SERVICE_TYPE_VIDEOMODE          "core.provider.video.mode"_ko

namespace VideoMode {
    
    class Request : public Interface_Request
    {
    public:
        static const UInt32 GetModes = Interface_Request::MAX + 0;
        static const UInt32 SetMode = Interface_Request::MAX + 1;
    };
    
#define Mode_Identifier         "identifier"        /* FlatInteger - required */
#define Mode_Width              "width"             /* FlatInteger - required */
#define Mode_Height             "height"            /* FlatInteger - required */
#define Mode_Type               "type"              /* FlatString - required */
#define Mode_Type_Text          "text"
#define Mode_Type_Graphical     "graphical"

    class SetMode : public Interface_Request
    {
    public:
        int port;
        int modeIdentifier;
        // TODO: A reply
    };

    class ModeList : public Interface_Response
    {
    public:
        FlatArray ports;
            // -> FlatArray modes
                // -> FlatDictionary properties
    };
}

#endif // __INTERFACE_VIDEOMODE_H__
