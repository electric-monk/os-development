#include "Console.h"
#include "CPU_intr.h"

ConsoleDriver *activeConsole, *panicConsole;

void Console_Init(void)
{
    panicConsole = activeConsole;
}

void Console_Panic(void)
{
    activeConsole = panicConsole;
    activeConsole->SetCursor(0, 0);
}

static void SimpleBlit(unsigned char *buffer, int pixelSpan, int lineSpan, int sourceX, int sourceY, int destX, int destY, int width, int height)
{
    if ((sourceX == destX) && (sourceY == destY))
        return;
    if ((width == 0) || (height == 0))
        return;
    // Work out directions
    int xDirection, yDirection;
    int startLineSource, startLineDest;
    int startPixelSource, startPixelDest;
    if (destY < sourceY) {
        // Copy from bottom to top, start at top
        yDirection = 1;
        startLineSource = sourceY;
        startLineDest = destY;
    } else {
        // Copy from top to bottom, start at bottom
        yDirection = -1;
        startLineSource = sourceY + height - 1;
        startLineDest = destY + height - 1;
    }
    if (destX < sourceX) {
        // Copy from right to left, start at left
        xDirection = 1;
        startPixelSource = sourceX;
        startPixelDest = destX;
    } else {
        // Copy from left to right, start at right
        xDirection = -1;
        startPixelSource = sourceX + width - 1;
        startPixelDest = destX + width - 1;
    }
    // Compute offsets
    int startByteSource = startPixelSource * pixelSpan;
    int startByteDest = startPixelDest * pixelSpan;
    int copyBytes = width * pixelSpan;
    int directionalLineSpan = yDirection * lineSpan;
    unsigned char *source = buffer + (startLineSource * lineSpan);
    unsigned char *dest = buffer + (startLineDest * lineSpan);
    // Actual copy
    for (int lineCount = height; lineCount != 0; lineCount--, source += directionalLineSpan, dest += directionalLineSpan) {
        unsigned char *sourceByte = source + startByteSource;
        unsigned char *destByte = dest + startByteDest;
        for (int byteCount = copyBytes; byteCount != 0; byteCount--, sourceByte += xDirection, destByte += xDirection)
            *destByte = *sourceByte;
    }
}

static void SimpleClear(unsigned char *buffer, int pixelSpan, int lineSpan, int x, int y, int w, int h)
{
    if ((w == 0) || (h == 0))
        return;
    buffer += lineSpan * y;
    buffer += pixelSpan * x;
    for (int line = 0; line < h; line++, buffer += lineSpan) {
        unsigned char *bufferLine = buffer;
        for (int pixelCount = pixelSpan * w; pixelCount != 0; pixelCount--, bufferLine++)
            *bufferLine = 0;
    }
}

static void SimpleInvert(unsigned char *buffer, int pixelSpan, int lineSpan, int x, int y, int w, int h)
{
    if ((w == 0) || (h == 0))
        return;
    buffer += lineSpan * y;
    buffer += pixelSpan * x;
    for (int line = 0; line < h; line++, buffer += lineSpan) {
        unsigned char *bufferLine = buffer;
        for (int pixelCount = pixelSpan * w; pixelCount != 0; pixelCount--, bufferLine++)
            *bufferLine = ~*bufferLine;
    }
}

void VGAConsoleDriver::SetMode(void *address, int width, int height, int widthSpan)
{
    _buffer = (unsigned char*)address;
    _width = width;
    _height = height;
    _widthSpan = widthSpan;
}

#define COLOUR_DISTANCE(r, g, b)        (((r) * (r)) + ((g) * (g)) + ((b) * (b)))
const SInt64 s_vgaValues[16] = {
    COLOUR_DISTANCE(0, 0, 0),
    COLOUR_DISTANCE(0, 0, 170),
    COLOUR_DISTANCE(0, 170, 0),
    COLOUR_DISTANCE(0, 170, 170),
    COLOUR_DISTANCE(170, 0, 0),
    COLOUR_DISTANCE(170, 0, 170),
    COLOUR_DISTANCE(170, 85, 0),
    COLOUR_DISTANCE(170, 170, 170),
    COLOUR_DISTANCE(85, 85, 85),
    COLOUR_DISTANCE(85, 85, 255),
    COLOUR_DISTANCE(85, 255, 85),
    COLOUR_DISTANCE(85, 255, 255),
    COLOUR_DISTANCE(255, 85, 85),
    COLOUR_DISTANCE(255, 85, 255),
    COLOUR_DISTANCE(255, 255, 85),
    COLOUR_DISTANCE(255, 255, 255),
};
static UInt8 ColourFromVGADefault(ConsoleDriver::Colour colour)
{
    int selected = 0;
    UInt64 foundDistance = 0xFFFFFFFFFFFFFFFF;
    SInt64 desired = COLOUR_DISTANCE(colour.red, colour.green, colour.blue);
    for (int i = 0; i < (sizeof(s_vgaValues) / sizeof(s_vgaValues[0])); i++) {
        SInt64 distance = desired - s_vgaValues[i];
        if (distance < 0)
            distance = -distance;
        if (distance < foundDistance) {
            selected = i;
            foundDistance = distance;
        }
    }
    return selected;
}

void VGAConsoleDriver::Set(char c, int x, int y, Colour foreground, Colour background)
{
    int offset = ((y * _widthSpan) + x) * 2;
    _buffer[offset + 0] = c;
    _buffer[offset + 1] = (ColourFromVGADefault(background) << 4) | ColourFromVGADefault(foreground);
}

int VGAConsoleDriver::Width(void)
{
    return _width;
}

int VGAConsoleDriver::Height(void)
{
    return _height;
}

void VGAConsoleDriver::Clear(int x, int y, int w, int h)
{
    SimpleClear(_buffer, 2, _widthSpan, x, y, w, h);
}

void VGAConsoleDriver::Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h)
{
    SimpleBlit(_buffer, 2, _widthSpan, xFrom, yFrom, xTo, yTo, w, h);
}
    
namespace ZeppFont {
#include "kernel.font.h"
}

#define PIXEL_HEIGHT        2
static void ToggleCursor(unsigned char *buffer, int pixelSpan, int widthSpan, int x, int y)
{
    SimpleInvert(buffer, pixelSpan, widthSpan, x * ZeppFont::width, ((y + 1) * ZeppFont::height) - (PIXEL_HEIGHT + 1), ZeppFont::width, PIXEL_HEIGHT);
}

void GraphicalConsoleDriver::SetMode(void *address, int width, int height, int pixelSpan, int widthSpan, int depth)
{
    _buffer = (unsigned char*)address;
    _width = width;
    _height = height;
    _pixelSpan = pixelSpan;
    _widthSpan = widthSpan;
    _depth = depth;
    _cursorX = 0;
    _cursorY = 0;
    _cursorEnabled = true;
    
    SimpleClear(_buffer, _pixelSpan, _widthSpan, 0, 0, _width, _height);
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, 0, 0);
}

static void SetPixel24(unsigned char *buffer, int widthSpan, int x, int y, UInt32 colour)
{
    buffer += (y * widthSpan) + (3 * x);
    buffer[0] = colour & 0xFF;
    buffer[1] = (colour >> 8) & 0xFF;
    buffer[2] = (colour >> 16) & 0xFF;
}

static void SetPixel32(unsigned char *buffer, int widthSpan, int x, int y, UInt32 colour)
{
    UInt32 *data = (UInt32*)buffer;
    data[(y * widthSpan) + x] = colour;
}

static UInt32 As24(ConsoleDriver::Colour const& colour)
{
    return (colour.red << 0) | (colour.green << 8) | (colour.blue << 16) | (0xFF << 24);
}

void GraphicalConsoleDriver::Set(char c, int x, int y, Colour foreground, Colour background)
{
    InterruptDisabler disabler;
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    int actualX = x * ZeppFont::width;
    int actualY = y * ZeppFont::height;
    int charWidth = ZeppFont::font[c].width;
    unsigned char *bitmap = ZeppFont::bitmap + (ZeppFont::font[c].offset * ZeppFont::height);
    for (int charY = 0; charY < ZeppFont::height; charY++) {
        unsigned char byte;
        // Prime first read
        int actualXcursor = actualX;
        for (int charX = 0; charX < charWidth; charX++, byte <<= 1) {
            if ((charX % 8) == 0) {
                byte = *bitmap;
                bitmap++;
            }
            UInt32 colour;
            if (byte & 0x80)
                colour = As24(foreground);
            else
                colour = As24(background);
            SetPixel24(_buffer, _widthSpan, actualXcursor, actualY, colour);
            actualXcursor++;
        }
        actualY++;
    }
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}

void GraphicalConsoleDriver::SetCursor(int x, int y)
{
    InterruptDisabler disabler;
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    _cursorX = x;
    _cursorY = y;
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}

void GraphicalConsoleDriver::GetCursor(int *x, int *y)
{
    if (x)
        *x = _cursorX;
    if (y)
        *y = _cursorY;
}

void GraphicalConsoleDriver::SetCursorEnabled(bool enabled)
{
    if ((_cursorEnabled && !enabled) || (!_cursorEnabled && enabled)) {
        _cursorEnabled = enabled;
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    }
}

int GraphicalConsoleDriver::Width(void)
{
    return _width / ZeppFont::width;
}

int GraphicalConsoleDriver::Height(void)
{
    return _height / ZeppFont::height;
}

void GraphicalConsoleDriver::Clear(int x, int y, int w, int h)
{
    InterruptDisabler disabler;
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    SimpleClear(_buffer, _pixelSpan, _widthSpan, x * ZeppFont::width, y * ZeppFont::height, w * ZeppFont::width, h * ZeppFont::height);
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}

void GraphicalConsoleDriver::Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h)
{
    InterruptDisabler disabler;
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    SimpleBlit(_buffer, _pixelSpan, _widthSpan, xFrom * ZeppFont::width, yFrom * ZeppFont::height, xTo * ZeppFont::width, yTo * ZeppFont::height, w * ZeppFont::width, h * ZeppFont::height);
    if (_cursorEnabled)
        ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}
