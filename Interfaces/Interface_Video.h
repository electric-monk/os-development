#ifndef __INTERFACE_VIDEO_H__
#define __INTERFACE_VIDEO_H__

#include "Interface.h"

#define SERVICE_TYPE_VIDEO              "core.provider.video"_ko

namespace Video {
    
    class Request : public Interface_Request
    {
    public:
        // To service
        static const UInt32 GetBuffer = Interface_Request::MAX + 0;         // GetBuffer -> Buffer
        static const UInt32 Dirty = Interface_Request::MAX + 1;             // DirtyRequest -> <nothing>
        static const UInt32 CheckBuffer = Interface_Request::MAX + 2;       // DirtyRequest -> Response
        // From service
//        static const UInt32 Buffer
    };
    
    class Response : public Interface_Response
    {
    public:
        // From service
        static const UInt32 Buffer = Interface_Response::MAX + 0;
    };
    
    /* REQUESTS - to service */
    
    class GetBuffer : public Request
    {
    public:
        // For a window, it specifies a desired size (the window server can reject it, though). Ignored by video cards as they have video mode settings.
        int requestedWidth, requestedHeight, requestedDepth;
    };
    
    class DirtyRequest : public Request // You may be able to cache the Buffer response and reuse it to ensure good performance (dirty requests may still be required, for double buffering/etc.)
    {
    public:
        // The area of the buffer that was dirtied
        int x, y, w, h;
        // Following the page with this request should be a full size framebuffer (note that as this is backed by VM, only the actual changes need be stored)
        void* Framebuffer(void) { return ((unsigned char*)this) + 4096; }
    };
    
    /* REQUESTS - from service */
    
    /* RESPONSES - from service */
    
    class Buffer : public Response
    {
    public:
        typedef enum {
            PixelFormat24RGB,
            PixelFormat24BGR,
            PixelFormat32RGBx,
            PixelFormat32BGRx,
        } PixelFormat;
        int width, height;
        int lineSpan, bytesPerPixel;
        PixelFormat format;
        // Following the page containing this response may be pages mapping in the video memory, which may be reused.
        void* Framebuffer(void) { return ((unsigned char*)this) + 4096; }
    };
    
    /* RESPONSES - to service */
    
}

#endif // __INTERFACE_VIDEO_H__
