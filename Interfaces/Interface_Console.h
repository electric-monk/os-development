#ifndef __INTERFACE_CONSOLE_H__
#define __INTERFACE_CONSOLE_H__

#include "Interface.h"

#define SERVICE_TYPE_CONSOLE            "core.provider.console"_ko

namespace Console {

    class Request : public Interface_Request
    {
    public:
        // To service
        static const UInt32 WriteCharacters = Interface_Request::MAX + 0;
        static const UInt32 GetCapabilities = Interface_Request::MAX + 1;
        // From service
        static const UInt32 SizeChanged = Interface_Request::MAX + 0;
    };
    
    class Response : public Interface_Response
    {
    public:
        static const UInt32 Capabilities = Interface_Response::MAX + 0;
    };

    /* REQUESTS - to service */
    class WriteCharacters : public Request
    {
    public:
        class Point {
        public:
            Point()
            {
                x = y = 0;
            }
            Point(Point const &pnt)
            {
                x = pnt.x;
                y = pnt.y;
            }
            int x, y;
        };
        typedef enum {
            mNone,
            mLeft,
            mRight,
            mUp,
            mDown,
            mScreenLeft,
            mScreenRight,
            mScreenTop,
            mScreenBottom,
        } Move;
        // Show the cursor graphic on screen
        class ShowCursor : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Cshc"_type; }
            void Initialise(bool visible)
            {
                type = Type();
                length = sizeof(ShowCursor);
                _value = visible;
            }
            bool Visible(void) {return _value;}
        private:
            bool _value;
        };
        // Move the cursor graphic on screen
        class SetCursor : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Ccur"_type; }
            void Initialise(Point point)
            {
                type = Type();
                length = sizeof(SetCursor);
                _point = point;
            }
            Point Location(void) { return _point; }
        private:
            Point _point;
        };
        // Move the actual "output" cursor
        class SetPrint : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Cprn"_type; }
            void Initialise(Point point)
            {
                type = Type();
                length = sizeof(SetPrint);
                _point = point;
            }
            Point Location(void) { return _point; }
        private:
            Point _point;
        };
        // Set colour output
        class SetColour : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Ccol"_type; }
            void Initialise(bool foreground, unsigned char red, unsigned char green, unsigned char blue)
            {
                type = Type();
                length = sizeof(SetColour);
                _foreground = foreground;
                _red = red;
                _green = green;
                _blue = blue;
            }
            bool Foreground(void) {return _foreground;}
            unsigned char Red(void) {return _red;}
            unsigned char Green(void) {return _green;}
            unsigned char Blue(void) {return _blue;}
        private:
            bool _foreground;
            unsigned char _red, _green, _blue;
        };
        // Set direction output
        class SetDirection : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Cdir"_type; }
            
            void Initialise(Move direction)
            {
                type = Type();
                length = sizeof(SetDirection);
                _direction = direction;
            }
            Move Direction(void) {return _direction;}
        private:
            Move _direction;
        };
        // Moving the "output" cursor
        class MoveDirection : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Cmod"_type; }
            
            void Initialise(Move direction)
            {
                type = Type();
                length = sizeof(SetDirection);
                _direction = direction;
            }
            Move Direction(void) {return _direction;}
        private:
            Move _direction;
        };
        // Move a block
        class MoveText : public FlatObject
        {
        public:
            static UInt32 Type(void) { return "Cmov"_type; }
            
            void Initialise(Point from, Point to, Point size)
            {
                type = Type();
                length = sizeof(MoveText);
                _from = from;
                _to =to;
                _size = size;
            }
            
            Point From(void) { return _from; }
            Point To(void) { return _to; }
            Point Size(void) { return _size; }
            
        private:
            Point _from, _to, _size;
        };
        
        FlatArray commands;
    };
    
    /* REQUESTS - from service */
    
    /* RESPONSES - from service */
    class Capabilities : public Interface_Response
    {
    public:
        int width, height;
        bool cursor;    // Can show/hide/move
        bool direction; // Can change direction of cursor from "left to right"
        bool colour;    // Can show colours
    };
    
    /* RESPONSES - to service */
    
}

#endif // __INTERFACE_CONSOLE_H__
