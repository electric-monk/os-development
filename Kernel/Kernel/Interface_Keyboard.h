#ifndef __INTERFACE_KEYBOARD_H__
#define __INTERFACE_KEYBOARD_H__

#include "Interface.h"

#define SERVICE_TYPE_KEYBOARD           "core.provider.keyboard"_ko

namespace Interface_Keyboard {
    
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
    
    typedef enum {
        // Main letter keys
        keyA = 'A',
        keyB = 'B',
        keyC = 'C',
        keyD = 'D',
        keyE = 'E',
        keyF = 'F',
        keyG = 'G',
        keyH = 'H',
        keyI = 'I',
        keyJ = 'J',
        keyK = 'K',
        keyL = 'L',
        keyM = 'M',
        keyN = 'N',
        keyO = 'O',
        keyP = 'P',
        keyQ = 'Q',
        keyR = 'R',
        keyS = 'S',
        keyT = 'T',
        keyU = 'U',
        keyV = 'V',
        keyW = 'W',
        keyX = 'X',
        keyY = 'Y',
        keyZ = 'Z',
        keySpace = ' ',
        keyReturn = 13,
        keyBackspace = 8,
        keyEscape = 1,
        key0 = '0',
        key1 = '1',
        key2 = '2',
        key3 = '3',
        key4 = '4',
        key5 = '5',
        key6 = '6',
        key7 = '7',
        key8 = '8',
        key9 = '9',
        // Cursor control
        keyLeft = 0x100,
        keyRight = 0x101,
        keyUp = 0x102,
        keyDown = 0x103,
        // Modifier keys
        keyShift = 0x200,
        keyControl = 0x201,
        keyAlt = 0x202,
        keyModifierLocationLeft = 0x10,
        keyModifierLocationRight = 0x20,
        keyModifierLocationMask = keyModifierLocationLeft | keyModifierLocationRight,
        keyShiftLeft = keyShift | keyModifierLocationLeft,
        keyShiftRight = keyShift | keyModifierLocationRight,
        keyControlLeft = keyControl | keyModifierLocationLeft,
        keyControlRight = keyControl | keyModifierLocationRight,
        keyAltLeft = keyAlt | keyModifierLocationLeft,
        keyAltRight = keyAlt | keyModifierLocationRight,
        // Numeric keypad
        keyPad0 = 0x300,
        keyPad1 = 0x301,
        keyPad2 = 0x302,
        keyPad3 = 0x303,
        keyPad4 = 0x304,
        keyPad5 = 0x305,
        keyPad6 = 0x306,
        keyPad7 = 0x307,
        keyPad8 = 0x308,
        keyPad9 = 0x309,
        keyPadStar = 0x310,
        keyPadPlus = 0x311,
        keyPadSlash = 0x312,
        keyPadMinus = 0x313,
        keyPadEnter = 0x314,
        keyPadPeriod = 0x315,
    } KEY;
    
    class Event : public Interface_Packet
    {
    public:
        bool keyDown;
        KEY keyValue;
    };
}

#endif // __INTERFACE_KEYBOARD_H__
